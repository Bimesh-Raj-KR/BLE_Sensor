# STM32_Communication

**STM32** is connected to **ESP32** via **UART1** and to **DHT11** sensor via **PC11** pin. In case of **STM32** reset due to **watchdog**, **ESP32** is also reset through **PC3** pin.

## Functions

1. On receiving **Ping** response, refreshes **watchdog.**
2. On receiving **Telemetry** request, reads and send over sensor readings.
3. On receiving **Timeout** response, updates **watchdog** and timeout **delay.**

## Features

Organized source codes in a single directory and further in subdirectories
Organized includes in a single directory and further in subdirectories
Use of **STM32CubeIDE**

## Prerequisites

**STM32CubeIDE** ≥ 1.18.1

## Further Requirements

1. Use **Build** tool of **STM32CubeIDE** to build the code.
2. Use **Run** tool of **STM32CubeIDE** to feed the code to **STM32**
3. Use **STM32CubeIDE** built in console or others like **Tera Term** to monitor output