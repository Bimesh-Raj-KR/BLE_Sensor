# Device_Manager

A cross-platform C project that list files opened by a remote device and allows categorization of the list on the basis of open files or PIDs

## Features

List all running processes and Opened files in **fileOpen.txt**

## Prerequisites

**Make**
**GCC** (for Device build)
**aarch64-linux-gnu-gcc** (for remote device build)

## Building with Make

### $ make list

Output: list

### $ make cross_list

Output: crosslist

## Requirements

Run **crosslist** on the remote device were you want the list