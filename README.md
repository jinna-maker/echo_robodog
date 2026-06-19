# 🐕🤖 ECHO RoboDOG

## หุ่นยนต์สุนัขอัจฉริยะเพื่อการตรวจวัดและตอบสนองด้านความปลอดภัย

### A Smart Robot Dog for Modular Sensing and Safety Response

![ECHO RoboDOG Design](images/echo01.png)

---

## 🌟 Motto

**Explore | Check | Help | Observe**

---

## 👤 Project Information

| Item          | Detail                                           |
| ------------- | ------------------------------------------------ |
| Student       | Jinna Arunplod                                   |
| School        | AIT International School                         |
| Project Type  | Invention / Robot Prototype                      |
| Project Theme | Robotics, Environmental Sensing, Safety Response |

---

## 📌 Project Overview

**ECHO RoboDOG** is a small smart robot dog prototype designed for **environmental sensing** and **safety response**.

The robot can detect abnormal air conditions such as:

* Poor air quality
* Gas
* Smoke
* Unusual smell
* Unsafe environmental conditions

When abnormal air is detected, the robot automatically changes its behavior to warn users.

It can:

* Stop moving
* Sit down
* Bark as a warning sound
* Show an alert face on the OLED screen

This project combines:

> **Robotics + Sensors + ESP32 + Servo Motors + Sound Alert + 3D Printing**

---

## 🇹🇭 Thai Project Title

**หุ่นยนต์สุนัขอัจฉริยะเพื่อการตรวจวัดและตอบสนองด้านความปลอดภัย**

---

## 🇬🇧 English Project Title

**A Smart Robot Dog for Modular Sensing and Safety Response**

---

## 🎯 Project Objectives

The main goal of this project is to create a small robot dog that can detect abnormal air conditions and respond in a simple and easy-to-understand way.

| No. | Objective                                                          |
| --- | ------------------------------------------------------------------ |
| 1   | Design and build a small robot dog prototype                       |
| 2   | Use MQ135 Gas Sensor to detect air quality or gas                  |
| 3   | Use ESP32 as the main controller                                   |
| 4   | Control robot movement using servo motors and PCA9685              |
| 5   | Create automatic responses such as stop, sit, bark, and alert face |
| 6   | Support STEM and Maker Education                                   |
| 7   | Design the robot as a modular platform for future upgrades         |

---

## 💡 Project Background

Some areas may contain hidden risks such as smoke, gas, poor air quality, or unsafe environmental conditions.

Direct inspection by humans may be dangerous, especially in:

* Closed rooms
* Laboratories
* Areas with unknown smell
* Areas with smoke or gas
* Unsafe or hard-to-reach locations

**ECHO RoboDOG** was created as a small scouting robot that can check the environment before humans enter.

The robot is designed to act like a small safety assistant. It can explore, check the air, and respond when it detects something abnormal.

---

## 🔄 Work Flow

The robot follows a simple safety response process:

```text
Sense  →  Detect  →  Alert  →  Respond
```

### Workflow Explanation

| Step    | Meaning                                                    |
| ------- | ---------------------------------------------------------- |
| Sense   | The MQ135 sensor checks the air quality                    |
| Detect  | ESP32 reads the sensor value                               |
| Alert   | If the value is abnormal, the robot shows warning behavior |
| Respond | The robot stops, sits, barks, and shows an alert face      |

---

## 🧰 Equipment and Components

The ECHO RoboDOG project uses electronic components, sensors, actuators, and 3D-printed parts to create a small smart robot dog prototype.

| No. | Component                      | Function                                              |
| --- | ------------------------------ | ----------------------------------------------------- |
| 1   | ESP32 Development Board        | Main controller of the robot                          |
| 2   | ESP32 Expansion Board          | Helps connect power, sensors, and output devices      |
| 3   | PCA9685 Servo Driver           | Controls multiple servo motors using I2C              |
| 4   | Servo Motors                   | Controls the robot legs and movement                  |
| 5   | MQ135 Gas / Air Quality Sensor | Detects abnormal air, gas, smoke, or poor air quality |
| 6   | OLED Display 0.96 inch         | Shows robot face, status, and alert messages          |
| 7   | Micro SD Card Module           | Stores sound files such as barking and alert sounds   |
| 8   | MAX98357A I2S Audio Amplifier  | Sends sound from ESP32 to the speaker                 |
| 9   | Speaker                        | Outputs barking or warning sounds                     |
| 10  | LiPo Battery / Power Supply    | Provides power to the robot                           |
| 11  | 12V to 5V Buck Converter       | Converts battery voltage to stable 5V                 |
| 12  | 3D-Printed Body Parts          | Robot dog body, legs, and head structure              |
| 13  | Jumper Wires / Cables          | Connects all components together                      |

---

## 🔌 GPIO Pin Connection

The following table shows the main GPIO connections used in this project.

| Device / Module      | Pin Name | ESP32 GPIO | Description                    |
| -------------------- | -------- | ---------- | ------------------------------ |
| OLED Display         | SDA      | GPIO 21    | I2C data                       |
| OLED Display         | SCL      | GPIO 22    | I2C clock                      |
| PCA9685 Servo Driver | SDA      | GPIO 21    | I2C data, shared with OLED     |
| PCA9685 Servo Driver | SCL      | GPIO 22    | I2C clock, shared with OLED    |
| MQ135 Sensor         | AO       | GPIO 32    | Analog air quality reading     |
| Micro SD Card Module | CS       | GPIO 5     | SPI chip select                |
| Micro SD Card Module | SCK      | GPIO 18    | SPI clock                      |
| Micro SD Card Module | MISO     | GPIO 19    | SPI data from SD card to ESP32 |
| Micro SD Card Module | MOSI     | GPIO 23    | SPI data from ESP32 to SD card |
| MAX98357A Amplifier  | DIN      | GPIO 27    | I2S audio data                 |
| MAX98357A Amplifier  | BCLK     | GPIO 14    | I2S bit clock                  |
| MAX98357A Amplifier  | LRC      | GPIO 12    | I2S left/right clock           |

---

## 🦾 Servo Motor Connection

The servo motors are connected to the **PCA9685 Servo Driver**, not directly to the ESP32.

This allows the robot to control multiple servos more smoothly and reduces the load on the ESP32.

| Robot Part       | PCA9685 Channel | Description                    |
| ---------------- | --------------- | ------------------------------ |
| Front Left Knee  | Channel 12      | Controls front left lower leg  |
| Front Left Hip   | Channel 13      | Controls front left upper leg  |
| Front Right Knee | Channel 14      | Controls front right lower leg |
| Front Right Hip  | Channel 15      | Controls front right upper leg |
| Back Left Knee   | Channel 0       | Controls back left lower leg   |
| Back Left Hip    | Channel 1       | Controls back left upper leg   |
| Back Right Knee  | Channel 2       | Controls back right lower leg  |
| Back Right Hip   | Channel 3       | Controls back right upper leg  |

---

## ⚡ Power Connection

The robot needs stable power because servo motors require more current than the ESP32 can provide.

### Recommended Power Flow

```text
Battery / 12V Power Source
        ↓
12V to 5V Buck Converter
        ↓
5V output to Servo Power / PCA9685 V+
        ↓
ESP32 powered through regulated 5V input
```

### Important Power Notes

* Do not power all servo motors directly from the ESP32.
* Use a 5V power source with enough current for the servos.
* Connect all GND pins together.
* ESP32 GND, PCA9685 GND, sensor GND, and power supply GND should share common ground.
* A capacitor can be added near the servo power line to reduce voltage drop.
* Low current adapters may cause ESP32 brownout reset when the servos move.

---

## 🤖 Robot Functions

ECHO RoboDOG can perform basic movement and safety response actions.

| Function           | Description                                                             |
| ------------------ | ----------------------------------------------------------------------- |
| Stand              | Robot stands in a ready position                                        |
| Walk               | Robot moves forward using servo movement                                |
| Sit                | Robot sits down                                                         |
| Sleep              | Robot lowers its body into a resting position                           |
| Give Hand          | Robot raises one front leg                                              |
| Stop               | Robot stops movement                                                    |
| Bark               | Robot plays barking sound                                               |
| Alert Face         | OLED shows warning expression                                           |
| Gas Detection      | MQ135 checks air quality                                                |
| Automatic Response | Robot stops, sits, barks, and shows alert when abnormal air is detected |

---

## 🧠 How It Works

ECHO RoboDOG uses the ESP32 as the main controller.

The MQ135 sensor reads air quality values and sends the data to the ESP32. If the sensor value is higher than the safety threshold, the robot changes its behavior automatically.

```text
MQ135 Sensor detects abnormal air
        ↓
ESP32 reads sensor value
        ↓
If value is above threshold
        ↓
Robot stops movement
        ↓
Robot sits down
        ↓
OLED shows alert face
        ↓
Speaker plays barking sound
```

---

## 🔊 Sound System

The robot uses a **Micro SD Card Module** and **MAX98357A I2S Audio Amplifier** to play sound effects.

Example sound files:

| File Name   | Purpose                        |
| ----------- | ------------------------------ |
| hello.wav   | Startup greeting sound         |
| barking.wav | Warning or alert barking sound |
| panting.wav | Robot dog idle sound           |

---

## 🖥️ OLED Display

The OLED display is used to show the robot's face and status.

Example display states:

* Normal face
* Happy face
* Alert face
* Gas warning message
* Robot status message

---

## 🧩 Modular Design

ECHO RoboDOG is designed as a modular robot platform.

The MQ135 sensor can be replaced or expanded with other sensors in the future, such as:

* Temperature sensor
* Humidity sensor
* Camera module
* Ultrasonic sensor
* Smoke sensor
* Motion sensor

This makes the robot useful for future learning, testing, and upgrading.

---


## 🚀 Future Improvements

Possible future upgrades include:

* Add more environmental sensors
* Add camera module for visual inspection
* Add Wi-Fi control or mobile app control
* Improve walking movement
* Add AI-based detection
* Improve battery life
* Create stronger 3D-printed body parts
* Add wireless data logging

---

## ✅ Conclusion

**ECHO RoboDOG** is a small smart robot dog prototype that combines robotics, sensors, sound, display, and 3D printing.

It can detect abnormal air conditions and respond automatically by stopping, sitting, barking, and showing an alert face.

This project demonstrates how a small robot can be used as a safety assistant and educational platform for STEM and Maker learning.

---

## 🙌 Creator

**Jinna Arunplod**
AIT International School
