
# Compiler
CC = g++

# All W Flags
CFLAGS = -Wall -Werror -Wextra -fanalyzer

# All .h Includes
INCLUDES = -ImultiStreamer

# All source files
source_files = $(wildcard *.cpp multiStreamer/*.cpp)

# All command substitutions
cmd_subs = $(shell pkg-config --cflags --libs gstreamer-1.0)

# Generate open file list
multi: $(source_files)
	$(CC) $(CFLAGS) $(INCLUDES) $(source_files) -o multiStream $(cmd_subs)