
# Compiler
CC = gcc

# All W Flags
CFLAGS = -Wall -Werror -Wextra -fanalyzer

# All .h Includes
INCLUDES = -IsenderFunctions

# All source files
source_files = $(wildcard *.c senderFunctions/*.c)

# All command substitutions
cmd_subs = $(shell pkg-config --cflags --libs gstreamer-1.0)

# Generate open file list
send: $(source_files)
	$(CC) $(CFLAGS) $(INCLUDES) $(source_files) -o send $(cmd_subs)