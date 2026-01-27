# G Streamer

A C project to send a file from a **sender** to **receiver** using **GStreamer pipelines** through **UDP_RTP** and also to create a **backup** for the file using the same **sender pipeline**

## Prerequisites

The code is configured to handle **audio** files   
Start **receiver** before running the **sender**

## Features

Every new send to the running receiver will append the received file   
Every new send will overwrite the prior backup   

Below are the piepline commands of sender and receiver that the code recreates  

gst-launch-1.0 filesrc location="<path to input file>" ! mpegaudioparse ! tee  name=splitter \  
    splitter. ! queue ! filesink location="backup.mp3" \  
    splitter. ! queue ! rtpgstpay ssrc=11111 ! udpsink host=127.0.0.1 port=5000 sync=false  

gst-launch-1.0 udpsrc port=5000 caps="application/x-rtp, media=(string)  application, clock-rate=(int)90000, encoding-name=(string)X-GST" !    rtpjitterbuffer mode=0 ! rtpgstdepay ! filesink location=ring.mp3

## Building with Make

### make send

Output: send

#### make receive

Output: receive