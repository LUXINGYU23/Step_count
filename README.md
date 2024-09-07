
# Pedometer Project Based on STC89C52RC

This project is a pedometer system designed using the STC89C52RC microcontroller and ADXL345 accelerometer sensor. The device counts steps and communicates with external devices via Bluetooth. Additionally, it displays the step count on an LCD screen. The system is built with modular hardware and software, ensuring expandability and efficient debugging.

## Table of Contents
1. [Project Overview](#project-overview)
2. [Hardware Components](#hardware-components)
3. [Circuit Design](#circuit-design)
4. [Software Structure](#software-structure)
5. [Algorithm for Step Detection](#algorithm-for-step-detection)
6. [Challenges and Solutions](#challenges-and-solutions)
7. [PCB Design](#pcb-design)
8. [Conclusion](#conclusion)

## Project Overview
- **Project Name**: Pedometer Design Based on STC89C52RC
- **Project Date**: August 2024
- **Author**: Xingyu Lu
- **Course**: Comprehensive Circuit Innovation Practice

This project aims to develop a digital pedometer leveraging the accelerometer sensor to track human movement and count steps. The device also integrates Bluetooth connectivity to transmit data to a mobile device, making it a compact and low-power solution for wearable health monitoring.

## Hardware Components
The hardware design integrates several key modules, including:
1. **STC89C52RC Microcontroller**: Serves as the core of the system.
2. **ADXL345 Accelerometer**: Captures acceleration in three axes (X, Y, Z) to determine step count.
3. **1602 LCD Display**: Displays real-time step count and additional information.
4. **JDY-31 Bluetooth Module**: Facilitates wireless communication with external devices.
5. **Power Management**: Includes a 7805 voltage regulator and filter capacitors to ensure stable power.
6. **CP2102 USB to Serial Module**: Allows for data transmission and debugging via USB.
7. **Button Matrix**: Provides user interface controls for system operation.

## Circuit Design
The circuit design is modular, with each subsystem playing a specific role. Here are the main aspects:
- **Power Circuit**: A 9V power supply is regulated to 5V using the 7805 regulator.
- **STC89C52 Microcontroller System**: Includes a clock circuit and reset function to manage the microcontroller.
- **Accelerometer Communication**: The ADXL345 sensor communicates via the I2C protocol with the microcontroller to gather acceleration data.
- **LCD Display**: Connected via parallel data lines to display real-time step count.
- **Bluetooth Module**: Facilitates data transfer using UART communication.

For detailed schematics, refer to the `/docs/circuit_schematic.pdf`.

## Software Structure
The software is modular and designed for real-time step detection and data communication. Key modules include:
1. **I2C Communication**: Manages data transmission between the microcontroller and ADXL345 sensor.
2. **Step Detection Algorithm**: Processes accelerometer data to identify and count steps based on threshold values.
3. **Display Module**: Updates the LCD with the current step count.
4. **Bluetooth Communication**: Transmits the step count to an external device.
5. **Button Input Handling**: Detects and processes button presses for mode switching.

## Algorithm for Step Detection
The core algorithm relies on accelerometer data from the ADXL345 sensor. It uses the following methods:
1. **Digital Filtering**: Filters out noise from raw accelerometer data using a moving average filter.
2. **Peak Detection**: Detects peaks in the acceleration data corresponding to step movements.
3. **Dynamic Thresholding**: Adjusts the step detection threshold based on user activity.
4. **Step Counting**: Counts steps when the acceleration crosses the dynamic threshold, ensuring accurate results across various walking speeds.

For more details on the algorithm, check the `step_detection.c` file in the repository.

## Challenges and Solutions
During development, several challenges were encountered:
- **Incorrect ADXL345 Pinout**: The accelerometer was incorrectly packaged, leading to reversed connections. This was resolved by mounting the sensor on the back of the PCB.
- **Low Sampling Rate**: The initial implementation suffered from missed steps due to a low sampling rate. This was fixed by moving the step detection logic inside the dynamic threshold update loop, ensuring real-time processing.

## PCB Design
The PCB design is compact and follows a two-layer layout:
- **Top Layer**: Hosts critical components like the microcontroller, accelerometer, and LCD connections.
- **Bottom Layer**: Handles power and ground distribution, ensuring minimal interference.
- **Signal Integrity**: Key signal lines (I2C, UART) are carefully routed to avoid noise.

For detailed design files, check the `/pcb_design` directory.

## Conclusion
This project successfully implements a reliable, low-power pedometer system using the STC89C52RC microcontroller and ADXL345 accelerometer. The device accurately counts steps and supports wireless communication, making it suitable for wearable applications. Future improvements could focus on enhancing battery life and integrating more sensors for comprehensive activity tracking.

## Repository Structure
```
/docs
  - circuit_schematic.pdf
  - pcb_layout.pdf
/src
  - main.c
  - step_detection.c
  - i2c.c
/pcb_design
  - pcb_layout.brd
  - pcb_schematic.sch
```
