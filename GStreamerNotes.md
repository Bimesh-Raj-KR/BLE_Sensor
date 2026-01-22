# GStreamer

GStreamer is a framework that is used for streaming data through a pipeline  
It has its uses in  audio and video streaming

A GStreamer pipeline is made up of different elements, primarily  
Source  
Decoder  
Encoder  
Payloader  
Depayloader  
Sink  

Additional elements can be added in accordance with data stream or file specifications  
Each element in the pipeline can also be given one or multiple properties to meet further requirements  

The source can be just be as simple as a test audio or video or a file or even a stream coming over to a port.  
Sink can also be like this  
The Source and the Snk are the two most important elements of a pipeline 

One or more data streams, like an audio stream and a video stream, can be multiplexed together and synced as a single stream  
Similarily a data stream containing an audio and a video can be demultiplexed into two seperate streams in the pipeline  

RTP is an application layer protocol that is used to packetize the data stream coming to sink  
This packet is then send over UDP to the receiver pipeline where it is depacketized  
UDP is the preferred as the transport layer as Data streaming is a low latency process where packet loss is of minor importance  

# Installing GStreamer

sudo apt update && sudo apt install -y \  
    libgstreamer1.0-dev \  
    libgstreamer-plugins-base1.0-dev \  
    libgstreamer-plugins-bad1.0-dev \  
    gstreamer1.0-plugins-base \  
    gstreamer1.0-plugins-good \  
    gstreamer1.0-plugins-bad \  
    gstreamer1.0-plugins-ugly \  
    gstreamer1.0-libav \  
    gstreamer1.0-tools \  
    gstreamer1.0-x \  
    gstreamer1.0-alsa \  
    gstreamer1.0-gl \  
    gstreamer1.0-gtk3 \  
    gstreamer1.0-qt5 \  
    gstreamer1.0-pulseaudio  

Below is the sender and receiver pipelines for sending a .mp3 file using GStreamer

gst-launch-1.0 filesrc location="sendAudio.mp3" ! \
    mpegaudioparse ! \
    rtpgstpay ssrc=11111 ! \
    udpsink host=127.0.0.1 port=5000 sync=false

gst-launch-1.0 udpsrc port=5000 caps="application/x-rtp, media=(string)application, clock-rate=(int)90000, encoding-name=(string)X-GST" ! \
    rtpjitterbuffer mode=0 ! \
    rtpgstdepay ! \
    filesink location=recvAudio.mp3
