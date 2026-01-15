
# Compiler
CC = gcc

# All W Flags
CFLAGS = -Wall -Werror -Wextra -fanalyzer

# All .h Includes
INCLUDES = -IclientFunctions

# All source files
source_files = $(wildcard *.c clientFunctions/*.c)

# Generate open file list
client: $(source_files)
	$(CC) $(CFLAGS) $(INCLUDES) $(source_files) -o client