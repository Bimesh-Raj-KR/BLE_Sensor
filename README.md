# Video Streamer


CPP project to Stream Live Video using a **Stream Uri** gotten and verified using Device discovery and **Onvifclient, Rtspclient and Gstreamer libraries.**
The project also contains another app that will create multiple GStreamer pipelines to forward the stream to multiple IPs.


## Prerequisites


Make sure all .sh files have **End of Line Sequence** as **LF**  
Run these commands in order for proper app functionality


1. To build Onvif shared library


cd OnvifClientLibrary/  
make clean  
make  
cd -


2. To build Rtsp shared library


Comment out lines **465-475** in **RtspClient/rtsp/rtsp_cln.cpp** to prevent port binding issues  


cd RtspClient/  
make -f RtspClientLibrary.mk clean  
make -f RtspClientLibrary.mk  
cd -  


3. Tell the OS to look for the libraries


export LD_LIBRARY_PATH=<path to dir>/OnvifClientLibrary:<path to dir>/OnvifClientLibrary/openssl/lib/linux:<path to dir>/RtspClient:<path to dir>/RtspClient/openssl/lib/linux:$LD_LIBRARY_PATH


4. Now make the app binary videoApp


make clean  
make  


5. Verify library linkage


ldd videoApp  -  **Check if any of the libraries are not linked(look for not found)**


Now run the binary **videoApp**




## Making Stream forwarding App


cd multiPipeline  
make


Now run **multiStreamer** app and enter the file path you want to stream


## Features


The app will probe for cameras in the local network and displays their IP.  
User given IP will be cross checked with this list to see if the camera is in the local network  
If present, the app will then try to get the different profiles in the camera  
The app tries to retrieve Profiles without authorisation first. If that fails the app tries again with authorisation  
After retrieving profiles, they are displayed on the terminal with important details  
From the list, the user can filter out a particular profile stream uri using resolution and encoding type  
This stream uri is then used to get live video stream from the camera  
The user is given options to pause stream, resume stream, reconnect and exit stream  
Every new reconnection will move the stream to another file as well as another udp port. The port value is incremented by 2 on every reconnection. This is RtspClient logic  


## Limitations


videoApp will cause 16,448 Bytes of memory leak if checked using valgrind  
This is due to lazy cleanup by Linux kernel and circular referencing by Gstreamer library and can usually be ignored.  

