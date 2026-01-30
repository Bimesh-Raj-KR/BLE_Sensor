
# Compiler
CC = gcc

# All W Flags
CFLAGS = -Wall -Werror -Wextra -fanalyzer

# All .h Includes
S_INCLUDES = -IsharedMemory -ISender -ISender/senderFunctions
R_INCLUDES = -IsharedMemory -IReceiver -IReceiver/receiverFunctions
C_INCLUDES = -IsharedMemory -ICompare -ICompare/compareFunctions

# All source files
send_src = $(wildcard sharedMemory/*.c Sender/*.c Sender/senderFunctions/*.c)
recv_src = $(wildcard sharedMemory/*.c Receiver/*.c \
			 Receiver/receiverFunctions/*.c)
comp_src = $(wildcard sharedMemory/*.c Compare/*.c Compare/compareFunctions/*.c)

# All command substitutions
cmd_subs = $(shell pkg-config --cflags --libs gstreamer-1.0)

# Generate binaries
send: $(send_src)
	$(CC) $(CFLAGS) $(S_INCLUDES) $(send_src) -o send $(cmd_subs)

receive: $(recv_src)
	$(CC) $(CFLAGS) $(R_INCLUDES) $(recv_src) -o receive $(cmd_subs)

options: $(recv_src)
	$(CC) $(CFLAGS) $(C_INCLUDES) $(comp_src) -o options