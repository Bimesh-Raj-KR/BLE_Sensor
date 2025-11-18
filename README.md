# BLE_Sensor

A BLE_Sensor project that allows Interboard communication between **STM32** and **ESP32** via **UART.** Furthermore, **STM32** reads temperature and Humidity data from **DHT11** sensor and **ESP32** is connected over **BLE** to a client. 

## Features

Organized source code in multiple directories   
Use of **PlatformIO** and **STM32CubeIDE**   

## Prerequisites

**PlatformIO** ≥ 3.3.4
**STM32CubeIDE** ≥ 1.18.1

## Physical Connections

### STM32 -> ESP32

1. **D2 -> TX2**
2. **D8 -> RX2**
3. **PC3 -> D13**
4. **GND -> GND**

### STM32 -> DHT11

1. **5V -> V+**
2. **GND -> V-**
3. **PC11 -> out**

### Further Requirements

1. Connect the Boards via ST-Link to the System and use console monitor to verify transmission
2. The LEDs on both boards are configured to blink every 20ms incase of reset.
3. Connect **PC3** pin to a Pull Down Resistor