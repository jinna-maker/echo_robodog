````md
---
title: ECHO RoboDOG
description: A Smart Robot Dog for Modular Sensing and Safety Response
---

# ECHO RoboDOG

## หุ่นยนต์สุนัขอัจฉริยะเพื่อการตรวจวัดและตอบสนองด้านความปลอดภัย  
### A Smart Robot Dog for Modular Sensing and Safety Response

---

## Project Information

**ECHO RoboDOG** is a smart robot dog prototype designed for environmental sensing and safety response.  
It can detect abnormal air conditions using an **MQ135 Gas Sensor** and respond with movement, sound, and facial expression.

The robot is designed as a small scouting companion that can help check unsafe areas before humans enter.

---

## Project Background

Some areas may contain hidden risks such as:

- smoke
- gas
- poor air quality
- abnormal smell
- unsafe environmental conditions

Direct inspection by humans may be dangerous in some situations.  
This project demonstrates how a small robot can help explore, detect, and warn users about possible environmental risks.

---

## Project Concept

The robot follows a simple safety response process:

```text
Sense → Detect → Alert → Respond
````

### Sense

The MQ135 sensor reads air quality or gas values from the environment.

### Detect

The ESP32 checks whether the sensor value is normal or above the defined threshold.

### Alert

If abnormal air conditions are detected, the robot changes to alert mode.

### Respond

The robot warns the user using movement, sound, and display.

---

## Main Robot Response

| Condition    | Robot Response                         |
| ------------ | -------------------------------------- |
| Normal air   | Stand, walk, smile face, panting sound |
| Abnormal air | Stop, sit, bark, angry or alert face   |

---

## Main Components

### ESP32 Microcontroller

The ESP32 is the main controller of the robot.
It reads sensor data, processes the condition, and controls the robot’s actions.

### MQ135 Gas Sensor

The MQ135 sensor detects air quality, gas, smoke, or abnormal smell.
It is the main sensor used for environmental checking.

### Servo Motor and PCA9685 Servo Driver

Servo motors control the movement of the robot dog.
The PCA9685 Servo Driver allows the ESP32 to control multiple servo motors more easily.

### OLED Screen

The OLED screen displays the robot’s face and system status.
It can show a happy face during normal mode and an alert face during warning mode.

### Micro SD Card Module

The Micro SD Card Module stores sound files such as barking, panting, and startup sounds.

### MAX98357A Amplifier and Speaker

The amplifier and speaker are used to play sound alerts, such as barking when abnormal air is detected.

### 3D Printed Robot Body

The robot body is designed using 3D modeling and produced with 3D printing.
It is designed to be easy to assemble, modify, and upgrade.

---

## Objectives

The objectives of this project are:

* to design and build a small robot dog prototype
* to detect air quality or abnormal gas using an MQ135 sensor
* to control the robot using ESP32
* to create movement responses using servo motors
* to add sound and display alerts
* to support STEM and Maker Education
* to design a modular robot platform for future upgrades

---

## Key Features

* Smart robot dog prototype
* Environmental sensing using MQ135
* Automatic response when abnormal air is detected
* Movement response such as stop, sit, and stand
* Sound alert using speaker
* OLED face display
* 3D printed body
* Modular design for future development
* Open-source learning platform

---

## Testing Summary

Initial testing showed that the robot can read values from the MQ135 gas sensor and use the data to decide its response.

When the air condition is normal, the robot stays in normal mode.
When the gas sensor value is higher than the defined threshold, the robot changes to alert mode.

In alert mode, the robot can stop, sit, bark, and show an alert face.
This shows that sensor data can be connected with robot behavior in an easy-to-understand way.

---

## Limitation

This project is currently a learning prototype.
The MQ135 sensor value can be affected by:

* temperature
* humidity
* gas type
* sensor warm-up time
* calibration setting

For real-world use, more testing and sensor calibration are required.

---

## Future Development

Future improvements may include:

* adding PM2.5 sensor
* adding smoke sensor
* adding temperature and humidity sensor
* adding CO2 sensor
* adding camera and image processing
* adding wireless communication
* adding mobile app or web dashboard
* improving walking stability
* improving robot body strength
* adding AI or Edge Computing for smarter analysis

---

## Educational Value

This project helps students learn about:

* robotics
* sensors
* ESP32 programming
* servo motor control
* sound system
* OLED display
* 3D design
* 3D printing
* problem solving
* STEM and Maker Education

---

## Conclusion

ECHO RoboDOG is a small smart robot dog that combines movement, sensing, sound, and automatic response.

It demonstrates how a robot can help detect abnormal air conditions and warn users in a simple and friendly way.
Although it is still a prototype, it can be developed further into a safety robot, air-quality scout robot, or open-source STEM learning platform.

---

## Project Credit

**Student:** Jinna Arunplod
**School:** AIT International School
**Project Theme:** Robotics, Air Sensing, Safety Response, STEM Learning

```
```
