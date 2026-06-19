/*
  Robot Dog FULL PROJECT
  ESP32 + PCA9685 + OLED + MQ135 D0 + SD Card + MAX98357A Speaker

  Modified MQ135 behavior:
    - Uses MQ135 D0 digital output on GPIO16 / D16
    - No analogRead() for MQ135
    - No automatic gas check in loop()
    - Interrupt only records D0 signal change
    - Robot checks air only when command is sent:
        GAS       -> show current D0 status only
        CHECK AIR -> check D0 once, then sit+bark if gas detected
        CLEAR AIR -> clear alert manually

  MQ135 wiring:
    MQ135 VCC -> 5V
    MQ135 GND -> ESP32 GND
    MQ135 D0  -> ESP32 GPIO16

  SD card files:
    /hello.wav
    /panting.wav
    /barking.wav

  Recommended WAV format:
    16-bit PCM, mono, 16000 Hz
*/

#include <Arduino.h>
#include <Wire.h>
#include <SPI.h>
#include <SD.h>
#include <Adafruit_PWMServoDriver.h>
#include <U8g2lib.h>
#include "driver/i2s.h"

// ---------------- I2C ----------------
#define SDA_PIN 21
#define SCL_PIN 22

// ---------------- OLED ----------------
U8G2_SH1106_128X64_NONAME_F_HW_I2C u8g2(
  U8G2_R2,
  /* reset=*/ U8X8_PIN_NONE,
  /* clock=*/ SCL_PIN,
  /* data=*/ SDA_PIN
);

// ---------------- PCA9685 ----------------
#define PCA_ADDR 0x40
Adafruit_PWMServoDriver pwm = Adafruit_PWMServoDriver(PCA_ADDR);

#define SERVO_FREQ 50
#define SERVOMIN 110
#define SERVOMAX 500

// PCA9685 channel map
#define FLK  12    // Front Left  Knee
#define FLH  13    // Front Left  Hip
#define FRK  14    // Front Right Knee
#define FRH  15    // Front Right Hip

#define BLK  0     // Back Left Knee
#define BLH  1     // Back Left Hip
#define BRK  2     // Back Right Knee
#define BRH  3     // Back Right Hip

// Real PCA9685 channels used by the 8 servos
const uint8_t SERVO_CH[8] = {FLK, FLH, FRK, FRH, BLK, BLH, BRK, BRH};

// ---------------- SD Card ----------------
#define SD_CS   5
#define SD_SCK  18
#define SD_MISO 19
#define SD_MOSI 23

// ---------------- MAX98357A I2S Speaker ----------------
#define I2S_DOUT 27
#define I2S_BCLK 14
#define I2S_LRC  12
#define I2S_PORT I2S_NUM_0

// ---------------- MQ135 DIGITAL D0 ----------------
#define MQ135_D0_PIN 16   // D16 = GPIO16

// Most MQ modules use: LOW = gas detected, HIGH = normal air.
// If your module is opposite, change this to false.
#define MQ135_ACTIVE_LOW true

volatile bool mq135Changed = false;
bool gasAlert = false;
bool isPlayingAudio = false;
bool stopCurrentAudio = false;
bool pendingBark = false;
bool sdReady = false;
bool startupHelloPlayed = false;

const unsigned long SERIAL_BAUD = 115200;
const size_t CMD_BUFFER_SIZE = 64;
char commandBuffer[CMD_BUFFER_SIZE];
size_t commandLength = 0;

void handleCommand(String cmd);
void pollSerialCommands();
void responsiveDelay(unsigned long durationMs);
void listFiles();

void IRAM_ATTR mq135ISR() {
  mq135Changed = true;
}

int readMQ135D0Raw() {
  return digitalRead(MQ135_D0_PIN);
}

bool isGasDetectedD0() {
  int state = readMQ135D0Raw();
  if (MQ135_ACTIVE_LOW) {
    return state == LOW;
  } else {
    return state == HIGH;
  }
}

// ---------------- Sound timing ----------------
unsigned long lastPanting = 0;
const unsigned long PANTING_INTERVAL = 10000;

// ---------------- Movement state ----------------
int currentAngles[16] = {90, 90, 90, 90, 90, 90, 90, 90, 90, 90, 90, 90, 90, 90, 90, 90};
bool isWalking = false;
bool isSleeping = false;

// ---------------- Servo helpers ----------------
int angleToPulse(int angle) {
  angle = constrain(angle, 0, 180);
  return map(angle, 0, 180, SERVOMIN, SERVOMAX);
}

void writeServo(uint8_t ch, int angle) {
  if (ch > 15) return;
  angle = constrain(angle, 0, 180);
  pwm.setPWM(ch, 0, angleToPulse(angle));
  currentAngles[ch] = angle;
}

void writeServoDirect(uint8_t ch, int angle) {
  if (ch > 15) return;
  angle = constrain(angle, 0, 180);
  pwm.setPWM(ch, 0, angleToPulse(angle));
}

void moveSmooth(const int targets[8], int steps = 25, int stepDelay = 20) {
  int start[8];

  for (int i = 0; i < 8; i++) {
    start[i] = currentAngles[SERVO_CH[i]];
  }

  for (int step = 1; step <= steps; step++) {
    for (int i = 0; i < 8; i++) {
      uint8_t ch = SERVO_CH[i];
      int angle = start[i] + ((targets[i] - start[i]) * step / steps);
      writeServoDirect(ch, angle);
    }
    responsiveDelay(stepDelay);
  }

  for (int i = 0; i < 8; i++) {
    uint8_t ch = SERVO_CH[i];
    currentAngles[ch] = targets[i];
    writeServo(ch, targets[i]);
  }
}

void moveOneSmooth(uint8_t ch, int target, int steps = 20, int stepDelay = 15) {
  if (ch > 15) return;
  int start = currentAngles[ch];
  target = constrain(target, 0, 180);

  for (int step = 1; step <= steps; step++) {
    int angle = start + ((target - start) * step / steps);
    writeServoDirect(ch, angle);
    responsiveDelay(stepDelay);
  }
  currentAngles[ch] = target;
  writeServo(ch, target);
}

void moveTwoSmooth(uint8_t ch1, int target1, uint8_t ch2, int target2, int steps = 20, int stepDelay = 15) {
  if (ch1 > 15 || ch2 > 15) return;
  int start1 = currentAngles[ch1];
  int start2 = currentAngles[ch2];

  target1 = constrain(target1, 0, 180);
  target2 = constrain(target2, 0, 180);

  for (int step = 1; step <= steps; step++) {
    int a1 = start1 + ((target1 - start1) * step / steps);
    int a2 = start2 + ((target2 - start2) * step / steps);
    writeServoDirect(ch1, a1);
    writeServoDirect(ch2, a2);
    responsiveDelay(stepDelay);
  }

  currentAngles[ch1] = target1;
  currentAngles[ch2] = target2;
  writeServo(ch1, target1);
  writeServo(ch2, target2);
}

// ---------------- OLED ----------------
void drawFace(bool isHappy, const char* modeText, int gasState = -1) {
  u8g2.clearBuffer();

  if (isHappy) {
    u8g2.drawCircle(35, 22, 8, U8G2_DRAW_UPPER_RIGHT | U8G2_DRAW_UPPER_LEFT);
    u8g2.drawCircle(93, 22, 8, U8G2_DRAW_UPPER_RIGHT | U8G2_DRAW_UPPER_LEFT);
    u8g2.drawCircle(64, 36, 12, U8G2_DRAW_LOWER_RIGHT | U8G2_DRAW_LOWER_LEFT);
  } else {
    u8g2.drawLine(25, 18, 45, 28);
    u8g2.drawLine(103, 18, 83, 28);
    u8g2.drawCircle(64, 50, 10, U8G2_DRAW_UPPER_RIGHT | U8G2_DRAW_UPPER_LEFT);
  }

  u8g2.drawTriangle(60, 33, 68, 33, 64, 38);

  u8g2.setFont(u8g2_font_6x10_tf);
  u8g2.setCursor(0, 54);
  u8g2.print("Mode:");
  u8g2.print(modeText);

  u8g2.setCursor(78, 54);
  u8g2.print("D0:");
  if (gasState >= 0) {
    u8g2.print(gasState == HIGH ? "HIGH" : "LOW");
  } else {
    u8g2.print("--");
  }

  u8g2.setCursor(0, 64);
  if (gasAlert) u8g2.print("ALERT! SIT + BARK");
  else u8g2.print("SAFE");

  u8g2.sendBuffer();
}

void drawText(const char* line1, const char* line2) {
  u8g2.clearBuffer();
  u8g2.setFont(u8g2_font_6x10_tf);
  u8g2.setCursor(0, 15);
  u8g2.print(line1);
  u8g2.setCursor(0, 30);
  u8g2.print(line2);
  u8g2.sendBuffer();
}

// ---------------- Audio ----------------
void setupI2S() {
  i2s_config_t i2s_config = {
    .mode = (i2s_mode_t)(I2S_MODE_MASTER | I2S_MODE_TX),
    .sample_rate = 16000,
    .bits_per_sample = I2S_BITS_PER_SAMPLE_16BIT,
    .channel_format = I2S_CHANNEL_FMT_ONLY_LEFT,
    .communication_format = I2S_COMM_FORMAT_STAND_I2S,
    .intr_alloc_flags = ESP_INTR_FLAG_LEVEL1,
    .dma_buf_count = 8,
    .dma_buf_len = 256,
    .use_apll = false,
    .tx_desc_auto_clear = true,
    .fixed_mclk = 0
  };

  i2s_pin_config_t pin_config = {
    .bck_io_num = I2S_BCLK,
    .ws_io_num = I2S_LRC,
    .data_out_num = I2S_DOUT,
    .data_in_num = I2S_PIN_NO_CHANGE
  };

  i2s_driver_install(I2S_PORT, &i2s_config, 0, NULL);
  i2s_set_pin(I2S_PORT, &pin_config);
  i2s_zero_dma_buffer(I2S_PORT);
}

bool playWav(const char* filename) {
  if (!sdReady) {
    Serial.println("SD not ready. Cannot play audio.");
    return false;
  }

  if (isPlayingAudio) {
    Serial.println("Audio already playing.");
    return false;
  }

  isPlayingAudio = true;
  stopCurrentAudio = false;

  Serial.print("Play: ");
  Serial.println(filename);

  File file = SD.open(filename);
  if (!file) {
    Serial.print("Cannot open ");
    Serial.println(filename);
    isPlayingAudio = false;
    return false;
  }

  // Check RIFF/WAVE header
  char riff[4];
  char wave[4];

  if (file.read((uint8_t*)riff, 4) != 4) {
    Serial.println("Bad WAV header: missing RIFF");
    file.close();
    isPlayingAudio = false;
    return false;
  }

  file.seek(8);
  if (file.read((uint8_t*)wave, 4) != 4) {
    Serial.println("Bad WAV header: missing WAVE");
    file.close();
    isPlayingAudio = false;
    return false;
  }

  if (memcmp(riff, "RIFF", 4) != 0 || memcmp(wave, "WAVE", 4) != 0) {
    Serial.println("Not a WAV file. Use real PCM WAV.");
    file.close();
    isPlayingAudio = false;
    return false;
  }

  uint16_t audioFormat = 0;
  uint16_t channels = 0;
  uint32_t sampleRate = 16000;
  uint16_t bitsPerSample = 0;
  bool foundFmt = false;
  bool foundData = false;
  uint32_t dataStart = 0;
  uint32_t dataSize = 0;

  file.seek(12);  // first chunk after RIFF/WAVE

  while (file.available()) {
    char chunkId[4];
    uint8_t sizeBytes[4];

    if (file.read((uint8_t*)chunkId, 4) != 4) break;
    if (file.read(sizeBytes, 4) != 4) break;

    uint32_t chunkSize =
      ((uint32_t)sizeBytes[0]) |
      ((uint32_t)sizeBytes[1] << 8) |
      ((uint32_t)sizeBytes[2] << 16) |
      ((uint32_t)sizeBytes[3] << 24);

    uint32_t chunkDataPos = file.position();

    if (memcmp(chunkId, "fmt ", 4) == 0) {
      uint8_t fmt[16];
      if (chunkSize < 16 || file.read(fmt, 16) != 16) {
        Serial.println("Bad WAV fmt chunk.");
        file.close();
        isPlayingAudio = false;
        return false;
      }

      audioFormat = fmt[0] | (fmt[1] << 8);
      channels = fmt[2] | (fmt[3] << 8);
      sampleRate = ((uint32_t)fmt[4]) | ((uint32_t)fmt[5] << 8) | ((uint32_t)fmt[6] << 16) | ((uint32_t)fmt[7] << 24);
      bitsPerSample = fmt[14] | (fmt[15] << 8);
      foundFmt = true;
    }
    else if (memcmp(chunkId, "data", 4) == 0) {
      dataStart = chunkDataPos;
      dataSize = chunkSize;
      foundData = true;
      break;
    }

    // WAV chunks are word-aligned
    file.seek(chunkDataPos + chunkSize + (chunkSize & 1));
  }

  if (!foundFmt || !foundData) {
    Serial.println("Bad WAV: fmt or data chunk not found.");
    file.close();
    isPlayingAudio = false;
    return false;
  }

  Serial.print("WAV sampleRate=");
  Serial.print(sampleRate);
  Serial.print(" bits=");
  Serial.print(bitsPerSample);
  Serial.print(" channels=");
  Serial.print(channels);
  Serial.print(" format=");
  Serial.println(audioFormat);

  if (audioFormat != 1 || bitsPerSample != 16 || channels != 1) {
    Serial.println("Unsupported WAV. Convert to PCM, 16-bit, mono.");
    file.close();
    isPlayingAudio = false;
    return false;
  }

  if (sampleRate < 8000 || sampleRate > 48000) {
    Serial.println("Unsupported sample rate. Using 16000.");
    sampleRate = 16000;
  }

  i2s_set_sample_rates(I2S_PORT, sampleRate);
  file.seek(dataStart);

  const int bufferSize = 512;
  uint8_t buffer[bufferSize];
  uint32_t remaining = dataSize;

  while (remaining > 0 && file.available()) {
    pollSerialCommands();

    if (stopCurrentAudio) {
      Serial.println("Audio stopped by new command.");
      break;
    }

    int toRead = remaining > bufferSize ? bufferSize : remaining;
    int bytesRead = file.read(buffer, toRead);
    if (bytesRead <= 0) break;

    remaining -= bytesRead;

    int16_t *samples = (int16_t *)buffer;

    for (int i = 0; i < bytesRead / 2; i++) {
      int32_t v = samples[i];
      v *= 4;  // volume boost

      if (v > 32767) v = 32767;
      if (v < -32768) v = -32768;

      samples[i] = (int16_t)v;
    }

    size_t bytesWritten = 0;
    i2s_write(I2S_PORT, buffer, bytesRead, &bytesWritten, portMAX_DELAY);
  }

  file.close();
  i2s_zero_dma_buffer(I2S_PORT);
  isPlayingAudio = false;
  stopCurrentAudio = false;
  return true;
}

// ---------------- I2C scanner ----------------
void i2cScan() {
  Serial.println("Scanning I2C...");
  byte count = 0;

  for (byte address = 1; address < 127; address++) {
    Wire.beginTransmission(address);
    byte error = Wire.endTransmission();

    if (error == 0) {
      Serial.print("Found I2C: 0x");
      if (address < 16) Serial.print("0");
      Serial.println(address, HEX);
      count++;
    }
  }

  if (count == 0) drawText("I2C ERROR", "Check SDA/SCL");
}

// ---------------- Poses ----------------
void standPose() {
  Serial.println("Action: STAND");
  isWalking = false;
  isSleeping = false;
  drawFace(true, "STAND");

  int stand[8] = {
    90, 90,
    90, 90,
    90, 90,
    90, 90
  };
  moveSmooth(stand, 25, 20);
}

void sitPose() {
  Serial.println("Action: SIT");
  isWalking = false;
  isSleeping = false;
  drawFace(true, "SIT");

  int sit[8] = {
    90, 90,
    90, 90,
    0,  90,
    180,90
  };
  moveSmooth(sit, 30, 20);
}

void sleepPose() {
  Serial.println("Action: SLEEP");
  isWalking = false;
  isSleeping = true;
  drawFace(true, "SLEEP");

  int sleepPos[8] = {
    0,   90,
    180, 90,
    0,   90,
    180, 90
  };
  moveSmooth(sleepPos, 35, 20);
}

void giveHand() {
  Serial.println("Action: GIVE HAND");
  isWalking = false;
  isSleeping = false;
  drawFace(true, "GIVE");

  int sit[8] = {
    90, 90,
    90, 90,
    0,  90,
    180,90
  };
  moveSmooth(sit, 25, 20);
  responsiveDelay(300);

  moveOneSmooth(FRK, 150, 20, 15);
  responsiveDelay(250);
  moveOneSmooth(FRH, 120, 20, 15);
}

void stopGiving() {
  Serial.println("Action: STOP GIVE");
  drawFace(true, "STAND");

  moveOneSmooth(FRH, 90, 15, 15);
  responsiveDelay(150);
  moveOneSmooth(FRK, 90, 15, 15);
}

void forwardStep() {
  moveTwoSmooth(FLK, 40, BRK, 140, 18, 15);
  if (!isWalking || gasAlert) return;
  responsiveDelay(150);

  moveTwoSmooth(FLH, 120, BRH, 60, 18, 15);
  if (!isWalking || gasAlert) return;
  responsiveDelay(150);

  moveTwoSmooth(FLK, 90, BRK, 90, 18, 15);
  if (!isWalking || gasAlert) return;
  responsiveDelay(180);

  moveTwoSmooth(FRK, 140, BLK, 40, 18, 15);
  if (!isWalking || gasAlert) return;
  responsiveDelay(150);

  moveTwoSmooth(FRH, 60, BLH, 120, 18, 15);
  if (!isWalking || gasAlert) return;
  responsiveDelay(150);

  moveTwoSmooth(FRK, 90, BLK, 90, 18, 15);
  if (!isWalking || gasAlert) return;
  responsiveDelay(200);

  moveTwoSmooth(FLH, 90, FRH, 90, 15, 15);
  if (!isWalking || gasAlert) return;
  moveTwoSmooth(BLH, 90, BRH, 90, 15, 15);
  if (!isWalking || gasAlert) return;
  responsiveDelay(200);
}

void startWalk() {
  Serial.println("Action: WALK");
  isSleeping = false;
  drawFace(true, "WALK");
  isWalking = true;
}

void stopWalk() {
  Serial.println("Action: STOP");
  isWalking = false;
  standPose();
}

// ---------------- MQ135 D0 command-only behavior ----------------
void printMQ135Status() {
  int state = readMQ135D0Raw();
  bool gasDetected = isGasDetectedD0();

  Serial.print("MQ135 D0 raw = ");
  Serial.print(state == HIGH ? "HIGH" : "LOW");
  Serial.print(" | status = ");
  Serial.println(gasDetected ? "GAS DETECTED" : "AIR NORMAL");

  Serial.print("Interrupt change flag = ");
  Serial.println(mq135Changed ? "YES" : "NO");

  drawFace(!gasDetected, gasDetected ? "GAS" : "AIR OK", state);
}

// --- UPDATED MANUAL CHECK AIR FUNCTION ---
void checkAirOnce() {
  int state = readMQ135D0Raw();
  bool gasDetected = isGasDetectedD0();

  mq135Changed = false;

  Serial.print("CHECK AIR: D0 = ");
  Serial.print(state == HIGH ? "HIGH" : "LOW");
  Serial.print(" -> ");
  Serial.println(gasDetected ? "GAS DETECTED" : "AIR NORMAL");

  if (gasDetected) {
    gasAlert = true;
    isWalking = false;

    Serial.println("GAS ALERT! Sit and bark.");
    drawFace(false, "ALERT", state);
    sitPose();
    drawFace(false, "BARK", state);

    // --- FORCE THE BARK TO PLAY ---
    if (isPlayingAudio) {
      stopCurrentAudio = true;
      pendingBark = true;
    } else {
      playWav("/barking.wav");
    }
    
  } else {
    gasAlert = false;
    Serial.println("Air normal. No automatic action.");
    drawFace(true, "AIR OK", state);
  }
}

void clearAirAlert() {
  gasAlert = false;
  mq135Changed = false;
  Serial.println("Gas alert cleared manually.");
  drawFace(true, "CLEAR");
}

void handlePanting() {
  if (gasAlert) return;
  if (isSleeping) return;
  if (isWalking) return;
  if (isPlayingAudio) return;

  unsigned long now = millis();
  if (now - lastPanting >= PANTING_INTERVAL) {
    lastPanting = now;
    playWav("/panting.wav");
  }
}

// ---------------- Serial commands ----------------
void printHelp() {
  Serial.println();
  Serial.println("Commands:");
  Serial.println("  SCAN");
  Serial.println("  STAND");
  Serial.println("  CENTER");
  Serial.println("  WALK");
  Serial.println("  STOP");
  Serial.println("  SIT");
  Serial.println("  SLEEP");
  Serial.println("  GIVE");
  Serial.println("  STOP G");
  Serial.println("  HAPPY");
  Serial.println("  ANGRY");
  Serial.println("  BARK");
  Serial.println("  PANT");
  Serial.println("  GAS       - read MQ135 D0 status only");
  Serial.println("  CHECK AIR - check MQ135 D0 once; sit+bark if gas detected");
  Serial.println("  CLEAR AIR - clear gas alert manually");
  Serial.println("  S0 90");
  Serial.println();
}

void listFiles() {
  File root = SD.open("/");
  if (!root) {
    Serial.println("Cannot open SD root.");
    return;
  }

  File file = root.openNextFile();

  while (file) {
    Serial.print("File: ");
    Serial.print(file.name());
    Serial.print("  Size=");
    Serial.println(file.size());
    file.close();
    file = root.openNextFile();
  }

  root.close();
}

void checkAudioFile(const char* filename) {
  File f = SD.open(filename);
  if (f) {
    Serial.print(filename);
    Serial.print(" FOUND size=");
    Serial.println(f.size());
    f.close();
  } else {
    Serial.print(filename);
    Serial.println(" NOT FOUND");
  }
}

void handleCommand(String cmd) {
  cmd.trim();
  cmd.toUpperCase();
  if (cmd.length() == 0) return;

  Serial.print("CMD: ");
  Serial.println(cmd);

  if (cmd == "SCAN") {
    i2cScan();
  }
  else if (cmd == "STAND" || cmd == "CENTER") {
    gasAlert = false;
    standPose();
  }
  else if (cmd == "WALK") {
    gasAlert = false;
    startWalk();
  }
  else if (cmd == "STOP") {
    gasAlert = false;
    stopWalk();
  }
  else if (cmd == "SIT") {
    sitPose();
  }
  else if (cmd == "SLEEP") {
    sleepPose();
  }
  else if (cmd == "GIVE") {
    giveHand();
  }
  else if (cmd == "STOP G") {
    stopGiving();
  }
  else if (cmd == "HAPPY") {
    drawFace(true, "HAPPY");
  }
  else if (cmd == "ANGRY") {
    drawFace(false, "ANGRY");
  }
  else if (cmd == "BARK") {
    if (isPlayingAudio) {
      Serial.println("Audio busy. Stopping current sound, then bark will play.");
      pendingBark = true;
      stopCurrentAudio = true;
      return;
    }
    drawFace(false, "BARK");
    playWav("/barking.wav");
  }
  else if (cmd == "PANT") {
    if (isPlayingAudio) {
      Serial.println("Audio busy. Try again after current sound.");
      return;
    }
    drawFace(true, "PANT");
    playWav("/panting.wav");
  }
  else if (cmd == "GAS") {
    printMQ135Status();
  }
  else if (cmd == "CHECK AIR" || cmd == "CHECK") {
    checkAirOnce();
  }
  else if (cmd == "CLEAR AIR" || cmd == "CLEAR") {
    clearAirAlert();
  }
  else if (cmd.startsWith("TH ")) {
    Serial.println("TH command ignored: D0 digital mode uses the blue potentiometer on MQ135 module, not code threshold.");
    Serial.println("Turn the module potentiometer to adjust gas trigger level.");
  }
  else if (cmd.startsWith("S")) {
    int spaceIndex = cmd.indexOf(' ');
    if (spaceIndex > 1) {
      int ch = cmd.substring(1, spaceIndex).toInt();
      int angle = cmd.substring(spaceIndex + 1).toInt();
      isWalking = false;
      drawFace(true, "SERVO");
      if (ch >= 0 && ch < 8) {
        moveOneSmooth(ch, angle, 20, 15);
        Serial.print("Set CH");
        Serial.print(ch);
        Serial.print(" to ");
        Serial.println(angle);
      } else {
        Serial.println("Servo channel must be 0 to 7.");
      }
    } else {
      Serial.println("Bad command. Use: S0 90");
    }
  }
  else {
    Serial.println("Unknown command.");
    printHelp();
  }
}

void setup() {
  Serial.begin(SERIAL_BAUD);
  Serial.setTimeout(10);
  responsiveDelay(1000);

  Serial.println();
  Serial.println("Robot Dog FULL PROJECT");
  Serial.println("PCA9685 + OLED + MQ135 D0 + SD + MAX98357A");

  pinMode(MQ135_D0_PIN, INPUT);
  attachInterrupt(digitalPinToInterrupt(MQ135_D0_PIN), mq135ISR, CHANGE);
  Serial.println("MQ135 D0 on GPIO16 / D16 interrupt ready.");
  Serial.println("No automatic gas check in loop. Use GAS or CHECK AIR command.");

  Wire.begin(SDA_PIN, SCL_PIN);
  Wire.setClock(100000);

  u8g2.begin();
  drawText("Robot Dog", "Booting...");

  pwm.begin();
  pwm.setPWMFreq(SERVO_FREQ);
  delay(20);

  setupI2S();

  SPI.begin(SD_SCK, SD_MISO, SD_MOSI, SD_CS);

  if (!SD.begin(SD_CS, SPI, 4000000)) {
    sdReady = false;
    Serial.println("SD card failed.");
    drawText("SD ERROR", "Check wiring/card");
  } else {
    sdReady = true;
    Serial.println("SD card OK.");
    Serial.println("=== FILE LIST ===");
    listFiles();

    Serial.println("=== AUDIO FILE CHECK ===");
    checkAudioFile("/hello.wav");
    checkAudioFile("/barking.wav");
    checkAudioFile("/panting.wav");
  }

  i2cScan();

  standPose();
  drawFace(true, "READY");

  printHelp();
}

void loop() {
  pollSerialCommands();

  if (!startupHelloPlayed) {
    startupHelloPlayed = true;
    if (sdReady) {
      drawFace(true, "HELLO");
      playWav("/hello.wav");
      drawFace(true, "READY");
    }
  }

  if (pendingBark && !isPlayingAudio) {
    pendingBark = false;
    drawFace(false, "BARK");
    playWav("/barking.wav");
  }

  // MQ135 is NOT checked automatically here.
  // The interrupt only sets mq135Changed=true.
  // Use command GAS or CHECK AIR to read D0 and act.

  handlePanting();

  if (isWalking && !gasAlert) {
    forwardStep();
  }
}

void pollSerialCommands() {
  while (Serial.available()) {
    char c = (char)Serial.read();

    if (c == '\r') continue;

    if (c == '\n') {
      if (commandLength > 0) {
        commandBuffer[commandLength] = '\0';
        handleCommand(String(commandBuffer));
        commandLength = 0;
      }
      continue;
    }

    if (commandLength < CMD_BUFFER_SIZE - 1) {
      commandBuffer[commandLength++] = c;
    } else {
      commandLength = 0;
      Serial.println("Command too long. Max 63 chars.");
    }
  }
}

void responsiveDelay(unsigned long durationMs) {
  unsigned long start = millis();
  while (millis() - start < durationMs) {
    pollSerialCommands();
    delay(1);

    if (gasAlert && isWalking) {
      isWalking = false;
      break;
    }
  }
}