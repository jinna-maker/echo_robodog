````md
<!-- README.md -->

<div align="center">

# 🐕🤖 ECHO RoboDOG  
## หุ่นยนต์สุนัขอัจฉริยะเพื่อการตรวจวัดและตอบสนองด้านความปลอดภัย  
### A Smart Robot Dog for Modular Sensing and Safety Response
![Design](images/echo01.png)
<br>



<br>

**Explore | Check | Help | Observe**

<br>

**Student:** Jinna Arunplod  
**School:** AIT International School  
**Project Type:** Invention / Robot Prototype  

</div>

---

## 📌 Project Overview

**ECHO RoboDOG** is a small smart robot dog prototype designed for **environmental sensing** and **safety response**.

The robot can detect abnormal air conditions such as:

- Poor air quality
- Gas
- Smoke
- Unusual smell
- Unsafe environmental conditions

When abnormal air is detected, the robot automatically changes its behavior to warn users.  
It can **stop, sit, bark, and show an alert face** on the OLED screen.

This project combines:

> Robotics + Sensors + ESP32 + Servo Motors + Sound Alert + 3D Printing

---

## 🇹🇭 ชื่อโครงงานภาษาไทย

> **หุ่นยนต์สุนัขอัจฉริยะเพื่อการตรวจวัดและตอบสนองด้านความปลอดภัย**

## 🇬🇧 English Project Title

> **A Smart Robot Dog for Modular Sensing and Safety Response**

---

## 🎯 Project Objectives

The main goal of this project is to create a small robot dog that can detect abnormal air conditions and respond in a simple and easy-to-understand way.

### Objectives

| No. | Objective |
|---|---|
| 1 | Design and build a small robot dog prototype |
| 2 | Use MQ135 Gas Sensor to detect air quality or gas |
| 3 | Use ESP32 as the main controller |
| 4 | Control robot movement using servo motors and PCA9685 |
| 5 | Create automatic responses such as stop, sit, bark, and alert face |
| 6 | Support STEM and Maker Education |
| 7 | Design the robot as a modular platform for future upgrades |

---

## 💡 Project Background

Some areas may contain hidden risks such as smoke, gas, poor air quality, or unsafe conditions.

Direct inspection by humans may be dangerous, especially in:

- Closed rooms
- Laboratories
- Areas with unknown smell
- Areas with smoke or gas
- Unsafe or hard-to-reach locations

ECHO RoboDOG was created as a small scouting robot that can check the environment before humans enter.

---

## 🔄 Work Flow

The robot follows a simple safety response process:

<div align="center">

```text
Sense  →  Detect  →  Alert  →  Respond
````

</div>

| Step        | Meaning                        | Robot Action                        |
| ----------- | ------------------------------ | ----------------------------------- |
| **Sense**   | Read air quality data          | MQ135 sensor checks the environment |
| **Detect**  | Check if the value is abnormal | ESP32 compares value with threshold |
| **Alert**   | Trigger warning mode           | Robot stops or sits                 |
| **Respond** | Warn the user                  | Robot barks and shows alert face    |

---

## 🤖 Robot Behavior

### ✅ Normal Condition

When the air condition is normal, the robot can:

* Stand
* Walk
* Show happy face
* Play panting sound
* Wait for command

### ⚠️ Bad Air Detected

When gas, smoke, or abnormal smell is detected, the robot can:

* Stop moving
* Sit down
* Bark
* Show angry or alert face
* Warn the user

---

## 🔩 Main Components

| Component                         | Function                                                    |
| --------------------------------- | ----------------------------------------------------------- |
| **ESP32 Microcontroller**         | Main controller for reading sensors and controlling actions |
| **MQ135 Gas Sensor**              | Detects air quality, gas, smoke, or unusual smell           |
| **Servo Motors**                  | Control robot movement and posture                          |
| **PCA9685 Servo Driver**          | Controls multiple servo motors                              |
| **OLED Screen**                   | Displays robot face and system status                       |
| **Micro SD Card Module**          | Stores sound files such as barking and panting              |
| **MAX98357A Amplifier + Speaker** | Plays sound alerts                                          |
| **3D Printed Body**               | Robot dog structure for easy assembly and modification      |

---

## 🧠 System Architecture

```text
MQ135 Gas Sensor
        ↓
      ESP32
        ↓
 ┌───────────────┬───────────────┬───────────────┐
 │               │               │               │
Servo Motors   OLED Screen     Speaker       SD Card
via PCA9685    Robot Face      Alert Sound    Sound Files
```

---

## 🧩 Key Features

### 🌟 Main Features

* Smart robot dog prototype
* Air quality and gas detection
* Automatic safety response
* Servo-based movement
* OLED face display
* Barking and alert sounds
* 3D printed robot body
* Modular design for future upgrades
* Suitable for STEM and Maker Education

---

## 🚀 Innovation

The key innovation of **ECHO RoboDOG** is the connection between:

> **Sensor Data + Robot Behavior + Sound Alert + Face Display**

Instead of only showing sensor values as numbers, the robot communicates through behavior that is easy for people to understand.

For example:

| Sensor Condition | Robot Response              |
| ---------------- | --------------------------- |
| Normal air       | Smile, stand, walk          |
| Bad air detected | Stop, sit, bark, angry face |

This makes the robot more friendly, interactive, and useful for learning.

---

## 🧪 Testing Result

From initial testing, the robot can read values from the MQ135 gas sensor and use the data to decide its response.

When the sensor value is normal, the robot stays in normal mode.

When the sensor value is higher than the defined threshold, the robot changes to alert mode.

### Alert Mode Actions

* Stop
* Sit
* Bark
* Show alert face

This shows that the robot can connect sensor data with physical movement and sound response.

---

## ⚠️ Limitations

This project is currently a learning prototype.

The MQ135 sensor value may be affected by:

* Temperature
* Humidity
* Type of gas
* Sensor warm-up time
* Calibration setting

For real-world use, more testing and sensor calibration are required.

---

## 🛠️ Future Development

Future improvements may include:

| Future Feature                    | Description                        |
| --------------------------------- | ---------------------------------- |
| **PM2.5 Sensor**                  | Detect dust particles              |
| **Smoke Sensor**                  | Improve smoke detection            |
| **Temperature & Humidity Sensor** | Add environmental monitoring       |
| **CO2 Sensor**                    | Detect carbon dioxide level        |
| **Camera Module**                 | Add visual monitoring              |
| **Wireless Communication**        | Remote control and monitoring      |
| **Mobile App / Web Dashboard**    | User-friendly control interface    |
| **AI / Edge Computing**           | Smarter analysis and response      |
| **Improved Walking Stability**    | Better robot movement              |
| **Data Logging**                  | Save air quality data for analysis |

---

## 📚 Educational Value

ECHO RoboDOG supports hands-on learning in:

* Robotics
* Electronics
* Sensor technology
* ESP32 programming
* Servo motor control
* Sound system
* OLED display
* 3D design
* 3D printing
* Problem solving
* STEM Education
* Maker Education

This project helps students understand how robots can sense the environment, make simple decisions, and respond automatically.

---

## 📂 Suggested Repository Structure

```text
ECHO-RoboDOG/
│
├── README.md
│
├── docs/
│   └── index.md
│
├── code/
│   └── Robot_Dog.ino
│
├── images/
│   ├── poster.png
│   ├── robot_photo.png
│   └── wiring_diagram.png
│
├── stl/
│   ├── body.stl
│   ├── head.stl
│   ├── hip.stl
│   ├── knee.stl
│   └── feet.stl
│
└── report/
    └── project_summary.pdf
```

---

## 🖼️ Project Images

Add your images in the `images/` folder and update the file names below.

```md
![ECHO RoboDOG Poster](images/poster.png)

![Robot Photo](images/robot_photo.png)

![Wiring Diagram](images/wiring_diagram.png)
```

---

## 💻 Code

The robot is programmed using **Arduino IDE** with ESP32.

Main functions include:

* Reading MQ135 sensor values
* Controlling servo motors
* Displaying OLED face
* Playing sound from SD card
* Triggering automatic alert behavior

Example robot actions:

```text
STAND
WALK
SIT
SLEEP
STOP
BARK
GIVE HAND
CHECK AIR
```

---

## 📝 Conclusion

**ECHO RoboDOG** demonstrates how a small robot can combine sensing, movement, sound, and automatic response to become a smart safety companion.

Although it is currently a prototype for learning and demonstration, it has strong potential for future development as:

* Environmental scout robot
* Air-quality alert robot
* Safety response robot
* Open-source STEM learning platform

---
![Design](images/echo02.png)

## 🙏 Credits

3D Model Download >> https://makerworld.com/en/@jinna.maker

**Project by:** Jinna Arunplod
**School:** AIT International School
**Project Theme:** Robotics, Air Sensing, Safety Response, STEM Learning

Documentation support: AI-assisted writing and editing.

---

<div align="center">

## 🐕 ECHO RoboDOG

### Explore | Check | Help | Observe

**Thank you for visiting this project!**

</div>
```
