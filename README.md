# List Files

A cross-platform C project that list files opened by a remote device and allows categorization of the list on the basis of open files or PIDs. A Linux service that counts all file descriptors of app **/usr/sbin/sshd** and prints it on syslog every **5 minutes**

## Features

List all running processes and Opened files in **fileOpenList.txt**


## Prerequisites

**Make**

**aarch64-linux-gnu-gcc** (for remote device build)

## Building with Make

### $ make List

Output: appList

# Requirements

Place **appList** in **/usr/local/bin**     
Place **appCount.sh** in **/usr/local/bin**    
Place **appRun.service** in **/etc/systemd/system**  

## Applications

Run **./appList -f fileName** on the remote device were you want to list PIDs that opened that file   
Run **./appList -a PID** on the remote device were you want to list files opened by that PID    
**Enable** and **start** **appRun.service** to get File descriptor count every **5 minutes**    
