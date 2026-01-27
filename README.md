# G Streamer

A C project to send a file from a **sender**  using **GStreamer pipelines** through **UDP_RTP** and receive it in another file format. Here the pipeline is designed to send an **mp4** file and receive it as an **mkv** file

## Prerequisites

The code is configured to handle **video** files   
Start **receiver** before running the **sender**

## Features

Every new send to the running receiver will append the received file   

Below are the piepline commands of sender and receiver that the code recreates  

gst-launch-1.0 -v filesrc location="earth.mp4" ! qtdemux !     queue ! h264parse !  rtph264pay config-interval=1 !     udpsink host=127.0.0.1 port=5000 sync=true 

gst-launch-1.0 -e udpsrc port=5000 caps="application/x-rtp, media=video, clock-rate=90000,  encoding-name=H264" !     rtpjitterbuffer latency=500 !     rtph264depay ! h264parse !     matroskamux ! filesink location=recv_earth.mkv

## Building with Make

### make send

Output: send

#### make receive

Output: receive