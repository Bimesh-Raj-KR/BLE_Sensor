# S record 

A C project to establish connection between a **server** and **client** using **socket communication** and parse **S record** formatted data within a specific **memory range**. The **client** sends the **file** and the **memory range** to the server and server parses the data within the memory range and output it. 

## Prerequisites

Have a file which contain data in **Motorola S Record format**

## Features

The client has a user interface which has the following options

1. Send File
2. Send Memory Range
3. Exit 

A new file , **parse.srec**, to store the received data will be created  
It will be deleted when connection is closed

## Building with Make

### make client

Output: client

#### make server

Output: server
