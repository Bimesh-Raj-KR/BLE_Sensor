################OPTION###################
OUTPUT = librtspclient.a
ARCH=arm64 #armv7, armv7s arm64
IOSVER=9.0
BASE=/Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin
SYSROOT=/Applications/Xcode.app/Contents/Developer/Platforms/iPhoneOS.platform/Developer/SDKs/iPhoneOS.sdk
CCOMPILE = $(BASE)/clang
CPPCOMPILE = $(BASE)/clang++
COMPILEOPTION += -c -pipe -g -arch $(ARCH)
COMPILEOPTION += -Xarch_$(ARCH) -miphoneos-version-min=$(IOSVER) -Xarch_$(ARCH) -isysroot$(SYSROOT)
COMPILEOPTION += -fobjc-nonfragile-abi -fobjc-legacy-dispatch -fPIC -Wall
COMPILEOPTION += -DIOS
COMPILEOPTION += -DMETADATA
COMPILEOPTION += -DREPLAY
COMPILEOPTION += -DOVER_HTTP
COMPILEOPTION += -DOVER_WEBSOCKET
LINK = $(BASE)/ar
LINKOPTION += -stdlib=libc++ -arch $(ARCH) -miphoneos-version-min=$(IOSVER) -Xarch_$(ARCH) -Wl,-syslibroot,$(SYSROOT) -Wl,-rpath,@executable_path/../Frameworks
LINKOPTION = -r $(OUTPUT)
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
	
