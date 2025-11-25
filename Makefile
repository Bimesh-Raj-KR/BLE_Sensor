
# Compiler
CC = gcc

# Cross Compiler
CRC = aarch64-linux-gnu-gcc

# All W Flags
CFLAGS = -Wall -Werror -Wextra -fanalyzer

# All .h Includes
INCLUDES = -IlistProcess -IcheckFunction

# All cross compiler .h Includes
CRINCLUDES = -static -IlistProcess -IcheckFunction

# All source files
source_files = $(wildcard *.c listProcess/*.c checkFunction/*.c)

# All directory paths are removed from source file names
file_names = $(notdir $(source_files))

# Generate open file list
list: $(source_files)
	$(CC) $(CFLAGS) $(INCLUDES) $(source_files) -o list

# Generate open file list for remote device
cross_list: $(source_files)
	$(CRC) $(CFLAGS) $(CRINCLUDES) $(source_files) -o crosslist