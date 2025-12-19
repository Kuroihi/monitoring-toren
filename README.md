```md
# Water Tank Monitoring System

## Overview
This project is a microcontroller-based water tank monitoring system designed to measure water level, display real-time status, trigger alerts, and log data for monitoring purposes. The system uses an ultrasonic sensor to calculate water height and percentage, provides visual and audible alerts, and records averaged data to an SD card.

This project was developed as an academic programming project with a focus on firmware design, sensor data processing, and hardware interfacing.

---

## Features
- Real-time water level measurement using an ultrasonic sensor  
- Water percentage calculation based on configurable tank height  
- OLED display showing tank height, water height, percentage, and system status  
- Visual and audible alerts for LOW, OKAY, and HIGH water levels  
- SD card data logging with averaged sensor readings  
- Adjustable tank height using hardware buttons  
- Noise reduction using multi-sample averaging  

---

## System Architecture

**Input**
- Ultrasonic sensor (distance measurement)
- Push buttons for tank height adjustment

**Processing**
- Distance-to-height conversion
- Percentage calculation
- Averaging of 30 sensor samples to reduce noise

**Output**
- OLED display (I2C)
- LEDs and buzzer (status indicators)
- SD card logging (SPI)

---

## Hardware Components
- Arduino-compatible microcontroller
- Ultrasonic Sensor (HC-SR04 or equivalent)
- OLED Display SSD1306 (128×64, I2C)
- SD Card Module (SPI)
- Buzzer
- Red and Green LEDs
- Push buttons
- Resistors and jumper wires

---

## Pin Configuration


| Buzzer : 3 
| Red LED : 4 
| Green LED : 5 
| Decrease Button : 6 
| Increase Button : 7 
| Ultrasonic Trigger : 8 
| Ultrasonic Echo : 9 
| SD Card Chip Select : 10 

---

## Software & Libraries
- Programming Language: C/C++
- Libraries:
  - SD.h
  - SPI.h
  - Wire.h
  - U8x8lib.h
- Development Environment: Arduino IDE

---

## Key Implementation Details
- Ultrasonic distance is converted to water height using:
```

water_height = tank_height - measured_distance

```
- Water percentage is constrained between 0–100%
- 30 sensor samples are collected and averaged to reduce noise
- Data is written to `log.txt` on the SD card after each averaging cycle
- OLED display updates in real time with system information

---

## Water Level Status Logic


| < 20% | LOW | Red LED + Buzzer |
| 20–85% | OKAY | Green LED |
| > 85% | HIGH | Red LED + Buzzer |

---

## Example SD Card Log Output
```

Height: 65 cm, Percentage: 65%
Height: 66 cm, Percentage: 66%

```

---

## Author
Kresna Anugrahayutya  
Electrical Engineering Undergraduate  
Universitas Padjadjaran
```
