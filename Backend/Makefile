
# Compiler
CC = gcc

# All W Flags
CFLAGS = -Wall -Werror -Wextra -fanalyzer

# All .h Includes
INCLUDES = -IcJSON -IjsonServer

# All source files
source_files = $(wildcard *.c cJSON/*.c jsonServer/*.c)

# Generate open file list
count: $(source_files)
	$(CC) $(CFLAGS) $(INCLUDES) $(source_files) -o jsonCount

# Clean up
clean:
	rm -f jsonCount
	rm -f count.json