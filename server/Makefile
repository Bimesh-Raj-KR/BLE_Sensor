
# Compiler
CC = gcc

# All W Flags
CFLAGS = -Wall -Werror -Wextra -fanalyzer

# All .h Includes
INCLUDES = -IserverFunctions -IparseSRecord -IwriteSRecord

# All source files
source_files = $(wildcard *.c serverFunctions/*.c parseSRecord/*.c \
				writeSRecord/*.c)

# Generate open file list
server: $(source_files)
	$(CC) $(CFLAGS) $(INCLUDES) $(source_files) -o server