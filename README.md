# Count_Files

A cross-platform C project that count files opened by an application in a remote device and allows categorization of the list on the basis of open files or PIDs

## Features

List all running processes and Opened files in **fileOpenList.txt**   
Hardcode the application on to macro **APP_NAME**   
The open file count will be logged to **syslog** every **5** minutes   

## Prerequisites

**Make**
**GCC** (for Device build)
**aarch64-linux-gnu-gcc** (for remote device build)

## Service File

[Unit]   
Description=Open File Count Service    
After=network.target  
  
[Service]  
ExecStart=**path to executable**  
Restart=always   
RestartSec=300   
User=**username**   
StandardOutput=syslog  
StandardError=syslog  
SyslogIdentifier=countFiles  
  
[Install]  
WantedBy=multi-user.target   

## Building with Make

### $ make appCount

Output: appCount

### $ make cross_list

Output: crossCount

## Requirements

Run **crosslist** on the remote device were you want the list  
**Enable** the service to start it at boot  
**Start** the service to get the count of open files   
