################OPTION###################
OUTPUT = librtspclient.so
CCOMPILE = gcc
CPPCOMPILE = g++
COMPILEOPTION += -c -O3 -fPIC -Wall
COMPILEOPTION += -DIOS
COMPILEOPTION += -DHTTPS
COMPILEOPTION += -DSRTP
COMPILEOPTION += -DRTSPS
COMPILEOPTION += -DMETADATA
COMPILEOPTION += -DREPLAY
COMPILEOPTION += -DOVER_HTTP
COMPILEOPTION += -DOVER_WEBSOCKET
#COMPILEOPTION += -DBACKCHANNEL
LINK = g++
LINKOPTION = -shared -o $(OUTPUT)
INCLUDEDIR += -I.
INCLUDEDIR += -I./bm
INCLUDEDIR += -I./http
INCLUDEDIR += -I./media
INCLUDEDIR += -I./rtp
INCLUDEDIR += -I./rtsp
INCLUDEDIR += -I/usr/local/include
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
	
