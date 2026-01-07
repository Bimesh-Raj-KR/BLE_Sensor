
# Compiler
CC = gcc

# All W Flags
CFLAGS = -Wall -Werror -Wextra -fanalyzer

# All .h Includes
INCLUDES = -IcJSON -IjsonServer

# All source files
source_files = $(wildcard *.c cJSON/*.c jsonServer/*.c)

# Generate open file list
time: $(source_files)
	$(CC) $(CFLAGS) $(INCLUDES) $(source_files) -o jsonTime

# Clean up
clean:
	rm -f jsonTime
	rm -f header.json
	rm -f time.json