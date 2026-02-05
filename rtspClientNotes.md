# Cross compiling shared library

To cross compile shared libraries in RtspClient, do the following  

chmod +x mklinks.sh  
./mklinks.sh  

If **-bash: ./mklinks.sh: /bin/sh^M: bad interpreter: No such file or directory** error is shown, Do either of the following

1. Run **sed -i 's/\r$//'  mklinks.sh**  
2. Open VS code and set **End of Line sequence** of **mklinks.sh** to **LF**  

To enable BACKCHANNEL compile option, uncommment **COMPILEOPTION += -DBACKCHANNEL** inside makefile

If BACKCHANNEL is enabled, run the following first

cd third   
tar jxf alsa-lib-1.1.6.tar.bz2   
cd alsa-lib-1.1.6   
./configure --enable-shared=yes   
make install   

Set compilation enviornment using

source /opt/fsl-imx-xwayland/5.4-zeus/environment-setup-aarch64-poky-linux

## Makefile changes

Change

CCOMPILE = gcc  
CPPCOMPILE = g++

To

CCOMPILE ?= $(CC)  
CPPCOMPILE ?= $(CXX)

Also change 

LINK = g++

To

LINK = $(CXX)

This will set the compiler in the source as the new compiler. 

Make sure these changes are made inside **RtspClientLibrary.mk** and **RtspClientLibrary-static.mk** 

Clean up the previous .o, .so and .a files

make -f RtspClientLibrary.mk clean   
make -f RtspClientLibrary-static.mk clean   

To get cross compiled **librtspclient.so**, Run

make -f RtspClientLibrary.mk    

To get static archive **librtspclient.a**, Run

make -f RtspClientLibrary-static.mk   
