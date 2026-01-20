# S record 

A C project to establish connection between a **server** and **client** using **socket communication**. The code can do the following :  

1. Send **SREC file** to server to parse
2. Send **Starting** and **Ending addresses** to the server for defining **memory range** for parsing
3. Send a **binary** file to server to convert it to **SREC** format and send it back

## Prerequisites

Have a file which contain data in **Motorola S Record format**   
Have a **Binary** File   
The **Binary** file should not be of size bigger than **16MB**

## Features

The client has a user interface which has the following options

1. Send File
2. Send Memory
3. Convert File
4. Exit 

A new file , **parse.srec**, to store the received data will be created on the **server** side   
It will be deleted when connection is closed   
A new file, **format.srec**, to store the converted binary file will be created on the **client**   

## Building with Make

### make client

Output: client

#### make server

Output: server
