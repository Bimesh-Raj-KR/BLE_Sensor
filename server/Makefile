
# Compiler
CC = gcc

# All W Flags
CFLAGS = -Wall -Werror -Wextra -fanalyzer

# All .h Includes
INCLUDES = -IserverFunctions -IparseSRecord

# All source files
source_files = $(wildcard *.c serverFunctions/*.c parseSRecord/*.c)

# Generate open file list
server: $(source_files)
	$(CC) $(CFLAGS) $(INCLUDES) $(source_files) -o server