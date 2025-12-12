
# Compiler
CC = gcc

# All W Flags
CFLAGS = -Wall -Werror -Wextra -fanalyzer

# All .h Includes
INCLUDES = -static -IserverFunctions

# All source files
source_files = $(wildcard *.c serverFunctions/*.c)

# Generate open file list
server: $(source_files)
	$(CC) $(CFLAGS) $(INCLUDES) $(source_files) -o server