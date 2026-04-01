LINPHONE_SDK_DIR = /home/bimesh/LinLib/linphone-sdk/build/linphone-sdk/desktop

INCLUDES = -I$(LINPHONE_SDK_DIR)/include -IcallerFunctions

LIBDIR   = $(LINPHONE_SDK_DIR)/lib

CC       = gcc
CFLAGS   = $(INCLUDES) -Wall -Wextra -O2 -g -D_REENTRANT
LDFLAGS  = -L$(LIBDIR) -Wl,-rpath,$(LIBDIR)

LIBS     = -llinphone -lmediastreamer2 -lortp -lbelle-sip -lbelr -lbctoolbox -lpthread -lm

.PHONY: all clean

all: call

call: callerFunctions/callerFunctions.c main.c
	$(CC) $(CFLAGS) $^ -o $@ $(LDFLAGS) $(LIBS)

clean:
	rm -f call echo_received.wav