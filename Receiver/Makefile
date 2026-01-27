
# Compiler
CC = gcc

# All W Flags
CFLAGS = -Wall -Werror -Wextra -fanalyzer

# All .h Includes
INCLUDES = -IreceiverFunctions

# All source files
source_files = $(wildcard *.c receiverFunctions/*.c)

# All command substitutions
cmd_subs = $(shell pkg-config --cflags --libs gstreamer-1.0)

# Generate open file list
receive: $(source_files)
	$(CC) $(CFLAGS) $(INCLUDES) $(source_files) -o receive $(cmd_subs)