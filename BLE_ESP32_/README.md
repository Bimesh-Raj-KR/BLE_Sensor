# ESP32_Communication

**ESP32** is connected to **STM32** via **UART2** and to a **BLE client** wirelessly via **Bluetooth.**

## Functions

1. On receiving **Ping** request, sends **Ping** response.
2. On receiving **Telemetry** response, send over sensor readings to **BLE client.**
3. On receiving **Timeout** request, get timeout delay from **BLE client** and send it over **UART**
4. On receiving **Version** response, send over **STM32CubeIDE** version to **BLE client.**


## Features

Organized source codes in a single directory   
Organized includes in a single directory Use of **PlatformIO**  

## Prerequisites

**PlatformIO** ≥ 3.3.4

### .ini requirements

**platform** = espressif32    
**board** = esp32doit-devkit-v1   
**framework** = arduino   
**monitor_speed** = 115200  

## Further Requirements

1. Use **Build** command of **PlatformIO** to build the code
2. Use **Upload** command of **PlatformIO** to feed the code to **ESP32**
3. Use **Monitor** command of **PlatformIO** to monitor output