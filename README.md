# G Streamer

A C project to send data from one Shared memory to another shared memory using **Gstreamer** pipleines via **udp_rtp**. Here the pipeline is designed to send and receive data in **mp3** format.

## Features

**Sender** handles putting data in **Sender SHM** and sending data from **Sender SHM** over **Gstreamer** pipeline   
**Receiver** handles both creating **Receiver SHM** and putting received data in **Receiver SHM**  
**Compare** provides options for **comparing** SHMs **Deleting** SHMs and Putting **Receiver SHM** data in an **mp3** file   

## Prerequisites

Create a **shmSendKey.txt** and **shmRecvKey.txt** and put their paths as file macros for both sender and receiver apps. This is a necessary step to generate **SHM keys**.  
Start **receiver** before **sender**  
Provide File path of the data which is put in **Sender SHM** to Sender, receiver and Compare as **command line arguments**   
Ensure all three of them receive the same path for one particular transimission    
**Delete** the SHMs after operations and checkings are done  

## Building with Make

### make send

Output: send

### make receive

Output: receive

### make options

Output: options