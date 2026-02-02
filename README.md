# G Streamer

A C project to send data from one Shared memory to another shared memory using **Gstreamer** pipleines via **udp_rtp**. An **mp3** data from SHM is send over **udp_rtp** to another SHM where it is stored as **wav**.

## Features

**Sender** has two seperate pipelines, one for putting mp3 data in **SHM** and another for sending **SHM** data over **udp_rtp** to **receiver**   
**Receiver** also has two seperate pipelines, one for receiving data stream from **sender** and putting that data in **SHM** and another for taking that data and making it into a **wav** file   
**shmsink** element creates shared memory using the given **socket** and **size**
**shmsrc** then access this memory using the socket descriptor  

These are the pipeline command equvalents of the app

### Sender

gst-launch-1.0 -v filesrc location=tune.mp3 ! mpegaudioparse ! mpg123audiodec ! audioconvert ! audio/x-raw,format=S16LE,channels=2,rate=44100,layout=interleaved ! shmsink socket-path=/tmp/source-shm shm-size=10000000 wait-for-connection=false

gst-launch-1.0 -v shmsrc socket-path=/tmp/source-shm do-timestamp=true ! audio/x-raw,format=S16LE,channels=2,rate=44100,layout=interleaved ! audioconvert ! rtpL16pay ! udpsink host=127.0.0.1 port=5000 sync=false

### Receiver

gst-launch-1.0 -v udpsrc port=5000 caps="application/x-rtp,media=audio,clock-rate=44100,encoding-name=L16,channels=2,payload=96" ! rtpL16depay ! capsfilter caps="audio/x-raw,format=S16BE,channels=2,rate=44100,layout=interleaved" ! shmsink socket-path=/tmp/test-shm shm-size=10000000 wait-for-connection=false

gst-launch-1.0 -v shmsrc socket-path=/tmp/test-shm is-live=true do-timestamp=true ! audio/x-raw,format=S16BE,channels=2,rate=44100,layout=interleaved ! audioconvert ! queue ! wavenc ! filesink location=shmOutput.wav

For testing pipeline commands

Run **Receiver** commands first  
**shmsink** takes precedence over **shmsrc**   
Replace **tune.mp3** with the required audio file path

## Prerequisites

Start **receiver** before **sender**  
Provide file path as hardcoded macro to the **sender**   
Ensure all three of them receive the same path for one particular transimission    
**Delete** the SHMs after operations and checkings are done  

## Building with Make

### make send

Output: send

### make receive

Output: receive
