################# SHARED OPTIONS #################
CCOMPILE = gcc
CPPCOMPILE = g++
LINK = g++

# Final Output Name
OUTPUT = videoApp

################# ONVIF SETTINGS #################
COMPILEOPTION = -g -c -Wall
COMPILEOPTION += -DPROFILE_C_SUPPORT -DPROFILE_G_SUPPORT -DTHERMAL_SUPPORT
COMPILEOPTION += -DCREDENTIAL_SUPPORT -DACCESS_RULES -DSCHEDULE_SUPPORT
COMPILEOPTION += -DRECEIVER_SUPPORT -DIPFILTER_SUPPORT -DDEVICEIO_SUPPORT
COMPILEOPTION += -DPROVISIONING_SUPPORT

INCLUDEDIR += -IOnvifClientLibrary/bm -IOnvifClientLibrary/http \
				-IOnvifClientLibrary/onvif
LIBDIRS += -LOnvifClientLibrary -LOnvifClientLibrary/openssl/lib/linux

################# VIDEO SETTINGS #################
COMPILEOPTION += -DSRTP -DRTSPS -DREPLAY -DOVER_HTTP -DOVER_WEBSOCKET

INCLUDEDIR += -IdeviceDiscovery -IdeviceGetStream
INCLUDEDIR += -IvideoStreamer -IRtspClient -IRtspClient/bm -IRtspClient/http 
INCLUDEDIR += -IRtspClient/media -IRtspClient/rtp -IRtspClient/rtsp 
INCLUDEDIR += -IRtspClient/ffmpeg/include -IRtspClient/openssl/include \
				-IRtspClient/libsrtp/include

LIBDIRS += -LRtspClient -LRtspClient/ffmpeg/lib/linux 
LIBDIRS += -LRtspClient/openssl/lib/linux -LRtspClient/libsrtp/lib/linux

# GStreamer integration
GST_CFLAGS = $(shell pkg-config --cflags gstreamer-1.0 gstreamer-app-1.0)
GST_LIBS   = $(shell pkg-config --libs gstreamer-1.0 gstreamer-app-1.0)
COMPILEOPTION += $(GST_CFLAGS)

################# OBJECTS #################
# Combined list of objects from both Makefiles
OBJS = main.o deviceDiscovery/deviceDiscovery.o \
deviceGetStream/deviceGetStream.o videoStreamer/videoStreamer.o

################# LIBRARIES #################
SHAREDLIB += -lonvifclient -lrtspclient -lpthread

# Conditional Backchannel Libs
ifneq ($(findstring BACKCHANNEL, $(COMPILEOPTION)),)
    SHAREDLIB += -lasound -lavformat -lswscale -lavcodec -lswresample \
				-lavutil -lopus -lx264 -lx265
endif

# SSL/Crypto (Ensuring they aren't duplicated)
SHAREDLIB += -lcrypto -lssl

ifneq ($(findstring SRTP, $(COMPILEOPTION)),)
    SHAREDLIB += -lsrtp2
endif

SHAREDLIB += $(GST_LIBS)

LINKOPTION = -g -o $(OUTPUT) -Wl,-rpath='$(shell pwd)/OnvifClientLibrary:\
			$(shell pwd)/OnvifClientLibrary/openssl/lib/linux'
# LINKOPTION = -g -o $(OUTPUT)

################# TARGETS #################

all: clean $(OUTPUT)

$(OUTPUT): $(OBJS)
	$(LINK) $(LINKOPTION) $(LIBDIRS) $(OBJS) $(SHAREDLIB)

clean: 
	rm -f $(OBJS)
	rm -f $(OUTPUT)

# Suffix Rules
.PRECIOUS:%.cpp %.cc %.cxx %.c %.m %.mm
.SUFFIXES: .cpp .cc .cxx .c .m .mm .o

.cpp.o:
	$(CPPCOMPILE) -c -o $*.o $(COMPILEOPTION) $(INCLUDEDIR) $*.cpp
.cc.o:
	$(CCOMPILE) -c -o $*.o $(COMPILEOPTION) $(INCLUDEDIR) $*.cc
.cxx.o:
	$(CPPCOMPILE) -c -o $*.o $(COMPILEOPTION) $(INCLUDEDIR) $*.cxx
.c.o:
	$(CCOMPILE) -c -o $*.o $(COMPILEOPTION) $(INCLUDEDIR) $*.c
.m.o:
	$(CCOMPILE) -c -o $*.o $(COMPILEOPTION) $(INCLUDEDIR) $*.m
.mm.o:
	$(CPPCOMPILE) -c -o $*.o $(COMPILEOPTION) $(INCLUDEDIR) $*.mm