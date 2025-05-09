# Line-Following Robot

This project involves the design and implementation of an autonomous line-following robot using an Arduino nano microcontroller. The robot is capable of following a dark line on a light surface using an 8-channel line sensor array and PID control for precise motor speed adjustments.

## Table of Contents
- Project Overview  

- Features  

- Hardware Requirements

- Software Requirements

- Installation and Setup

- Usage

- Code Structure

## Project Overview

The line-following robot is designed to autonomously navigate along a predefined path marked by a dark line on a light surface. The robot uses an 8-channel line sensor array to detect the line and adjust its movement accordingly. The project aims to demonstrate the application of control algorithms, sensor integration, and embedded systems programming.

## eatures

- Autonomous line following using PID control

- Color calibration for accurate sensor readings under varying lighting conditions

- User interface with buttons for speed selection and LED indicators for status feedback

- EEPROM storage for calibration data persistence

- Real-time sensor polling with low latency (<1 ms)

## Hardware Requirements

- Arduino Uno or compatible microcontroller

- GA25 DC Motors (15A, 7.2-16V)

- BTS7960 motor driver

- LM2596S voltage regulator

- 8-channel line sensor array

- RGB LED module

- Push-button modules

- Buzzer

- Pin lipo 3s 12V for power supply

## Software Requirements

- Arduino IDE (version 1.8.x or later)

- AVR Libc (included with Arduino IDE)

## Installation and Setup

### 1. Software Installation:

- Install the Arduino IDE from the official website.

- Open the project code in the Arduino IDE.

### 2. Configuration:

- Ensure the correct board and port are selected in the Arduino IDE.

- Upload the code to the Arduino.

### 3. Calibration:

- Place the robot on the track and power it on.

- Press the calibration button to start the calibration process.

- Move the robot over the line and background to capture sensor values.

- Save the calibration data to EEPROM.

## Usage

### 1. Starting the Robot:

- Press one of the speed selection buttons to start the robot with the desired speed profile.

### 2. Calibrating Sensors:

- During calibration mode, move the robot over the line and background.

- The robot will automatically adjust the sensor thresholds.

### 3. Adjusting Parameters:

- Modify the PID constants in the code to fine-tune the robot's performance.

## Code Structure

The code is organized into several functions:

- **setup()**: Initializes hardware and loads calibration data.

- **loop()**: Main control loop with state machine for robot behavior.

- **read_sensor()**: Reads and processes sensor data for PID control.

- **speed_run()**: Sets motor speeds based on PID output.

- **handleAndSpeed()**: Applies PID adjustments to motor speeds.

- **updateLine()**: Updates calibration values during setup.

- **RGB()**: Controls the RGB LED for status indication.
