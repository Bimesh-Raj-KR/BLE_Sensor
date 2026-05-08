################OPTION###################
OUTPUT = librtspclient.so
NDK=/home/android-ndk-r25c
API=23
PLATFORM=aarch64
TOOLCHAIN=$(NDK)/toolchains/llvm/prebuilt/linux-x86_64/bin
SYSROOT=$(NDK)/toolchains/llvm/prebuilt/linux-x86_64/sysroot
ifneq ($(findstring armv7a, $(PLATFORM)),)
TARGET=$(PLATFORM)-linux-androideabi
RANLIB=$(TOOLCHAIN)/arm-linux-androideabi-ranlib
LINK = $(TOOLCHAIN)/clang++
endif
ifneq ($(findstring aarch64, $(PLATFORM)),)
TARGET=$(PLATFORM)-linux-android
RANLIB=$(TOOLCHAIN)/$(TARGET)-ranlib
LINK = $(TOOLCHAIN)/clang++
endif
CCOMPILE = $(TOOLCHAIN)/clang
CPPCOMPILE = $(TOOLCHAIN)/clang++
COMPILEOPTION += -fPIC -DANDROID --sysroot=$(SYSROOT)
COMPILEOPTION += -c -target $(TARGET)$(API) -O3 -fPIC -Wall
COMPILEOPTION += -DEPOLL
COMPILEOPTION += -DMETADATA
COMPILEOPTION += -DREPLAY
COMPILEOPTION += -DOVER_HTTP
COMPILEOPTION += -DOVER_WEBSOCKET
LINKOPTION += -target $(TARGET)$(API) -shared -o $(OUTPUT)
INCLUDEDIR += -I.
INCLUDEDIR += -I./bm
INCLUDEDIR += -I./http
INCLUDEDIR += -I./media
INCLUDEDIR += -I./rtp
INCLUDEDIR += -I./rtsp
INCLUDEDIR += -I./ffmpeg/include
INCLUDEDIR += -I./openssl/include
INCLUDEDIR += -I./libsrtp/include
LIBDIRS = 
OBJS += bm/base64.o
OBJS += bm/hqueue.o
OBJS += bm/linked_list.o
OBJS += bm/net_util.o
OBJS += bm/ppstack.o
OBJS += bm/rfc_md5.o
OBJS += bm/sha256.o
OBJS += bm/sys_buf.o
OBJS += bm/sys_log.o
OBJS += bm/sys_os.o
OBJS += bm/util.o
OBJS += bm/word_analyse.o
OBJS += http/http_cln.o
OBJS += http/http_parse.o
OBJS += rtp/aac_rtp_rx.o
OBJS += rtp/h264_rtp_rx.o
OBJS += rtp/h265_rtp_rx.o
OBJS += rtp/mjpeg_rtp_rx.o
OBJS += rtp/mjpeg_tables.o
OBJS += rtp/mpeg4.o
OBJS += rtp/mpeg4_rtp_rx.o
OBJS += rtp/pcm_rtp_rx.o
OBJS += rtp/rtp_rx.o
OBJS += rtsp/rtsp_cln.o
OBJS += rtsp/rtsp_parse.o
OBJS += rtsp/rtsp_rcua.o
OBJS += rtsp/rtsp_util.o

ifneq ($(findstring OVER_WEBSOCKET, $(COMPILEOPTION)),)
OBJS += bm/ws.o
endif

ifneq ($(findstring BACKCHANNEL, $(COMPILEOPTION)),)
OBJS += media/alsa.o
OBJS += media/audio_capture.o
OBJS += media/audio_capture_linux.o
OBJS += media/audio_encoder.o
OBJS += media/avcodec_mutex.o
OBJS += media/media_codec.o
OBJS += rtsp/rtsp_backchannel.o
endif

SHAREDLIB +=

APPENDLIB = 

################OPTION END################

$(OUTPUT):$(OBJS) $(APPENDLIB)
	$(LINK) $(LINKOPTION) $(LIBDIRS) $(OBJS) $(SHAREDLIB) $(APPENDLIB) 

clean: 
	rm -f $(OBJS)
	rm -f $(OUTPUT)
all: clean $(OUTPUT)
.PRECIOUS:%.cpp %.cc %.cxx %.c %.m %.mm
.SUFFIXES:
.SUFFIXES: .cpp .cc .cxx .c .m .mm .o

.cpp.o:
	$(CPPCOMPILE) -c -o $*.o $(COMPILEOPTION) $(INCLUDEDIR)  $*.cpp
	
.cc.o:
	$(CCOMPILE) -c -o $*.o $(COMPILEOPTION) $(INCLUDEDIR)  $*.cc

.cxx.o:
	$(CPPCOMPILE) -c -o $*.o $(COMPILEOPTION) $(INCLUDEDIR)  $*.cxx

.c.o:
	$(CCOMPILE) -c -o $*.o $(COMPILEOPTION) $(INCLUDEDIR) $*.c

.m.o:
	$(CCOMPILE) -c -o $*.o $(COMPILEOPTION) $(INCLUDEDIR) $*.m

.mm.o:
	$(CPPCOMPILE) -c -o $*.o $(COMPILEOPTION) $(INCLUDEDIR) $*.mm
	
