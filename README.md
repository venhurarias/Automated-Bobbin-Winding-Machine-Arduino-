# Automated Bobbin Winding Machine

This project is an Arduino-based **automated bobbin winding machine** designed to efficiently wind thread onto bobbins with controlled speed, direction, and stopping conditions. It integrates motor control, sensors, and user inputs to ensure consistent and precise winding.

---

## Project Overview

The system automates the bobbin winding process by controlling:

- Thread winding motor  
- Direction and speed of rotation  
- Stopping mechanism when bobbin is full  
- User input for operation control  

It is designed to:
- improve consistency in bobbin winding  
- reduce manual labor  
- prevent overfilling and thread damage  
- provide a reliable and repeatable process  

---

## Features

### 🔄 Automated Winding
- Automatically rotates the bobbin using a motor  
- Ensures consistent winding speed  
- Reduces manual intervention  

---

### ⚙️ Motor Control
- Controls motor:
  - ON / OFF  
  - Direction (if applicable)  
- Can adjust winding behavior based on system logic  

---

### 🧵 Full Bobbin Detection
- Detects when bobbin is full using sensor or condition  
- Automatically stops the motor  
- Prevents thread overflow  

---

### 🔘 User Controls
- Start button → begins winding  
- Stop button → stops operation  
- Optional reset or mode selection  

---

### 🚨 Safety Mechanism
- Stops system when:
  - bobbin is full  
  - emergency condition is triggered  
- Prevents motor overheating and thread breakage  

---

## System Workflow

### 1. Idle State
- System waits for user input  
- Motor remains OFF  

---

### 2. Start Operation
- User presses start button  
- Motor begins rotating  
- Thread starts winding onto bobbin  

---

### 3. Winding Process
- System continuously runs motor  
- Monitors:
  - winding condition  
  - sensor feedback  

---

### 4. Full Detection
- When bobbin is full:
  - motor stops automatically  
  - system indicates completion  

---

### 5. Stop / Reset
- User can manually stop system  
- Ready for next bobbin cycle  

---

## Hardware Components

- Arduino (Uno / Mega)  
- DC Motor or Stepper Motor  
- Motor Driver (L298N or similar)  
- Bobbin Holder / Mechanical Assembly  
- Sensor (limit switch / optical / IR)  
- Push Buttons (Start / Stop)  
- Power Supply  

---

## Pin Configuration

*(Adjust based on your actual code)*

| Component        | Arduino Pin |
|------------------|------------|
| Motor Control    | Digital Pin |
| Start Button     | Digital Pin |
| Stop Button      | Digital Pin |
| Sensor Input     | Digital Pin |

---

## Wiring Overview

### Motor
- Connected via motor driver  
- Requires external power supply  

---

### Sensor
- Detects bobbin full condition  
- Connected to digital input  

---

### Buttons
- Connected using INPUT_PULLUP  
- Active LOW when pressed  

---

## Control Logic

### Start Condition
- Start button pressed → motor ON  

---

### Stop Condition
- Stop button pressed → motor OFF  
- Bobbin full detected → motor OFF  

---

## Notes

- Ensure proper motor driver is used for load handling  
- Adjust motor speed depending on thread type  
- Sensor placement is critical for accurate detection  
- Mechanical alignment affects winding quality  

---

## Limitations

- No speed feedback (open-loop system)  
- No IoT or remote monitoring  
- Requires calibration for different thread types  
- Sensor accuracy depends on placement  

---

## Summary

This project demonstrates a **basic automated bobbin winding system** that combines:

- motor control  
- sensor-based stopping  
- user interaction  
- simple automation logic  

It is suitable for:

- textile applications  
- small-scale manufacturing  
- automation prototypes  
- embedded system projects  
