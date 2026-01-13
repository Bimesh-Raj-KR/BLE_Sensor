
# Compiler
CC = gcc

# All W Flags
CFLAGS = -Wall -Werror -Wextra -fanalyzer

# All .h Includes
INCLUDES = -IcgiParser

# All source files
source_files = $(wildcard *.c cgiParser/*.c)

# Generate open file list
parse: $(source_files)
	$(CC) $(CFLAGS) $(INCLUDES) $(source_files) -o parser.cgi

# Clean up
clean:
	rm -f parser.cgi