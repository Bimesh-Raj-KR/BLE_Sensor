
# Compiler
CC = gcc

# Cross Compiler
CRC = aarch64-linux-gnu-gcc

# All W Flags
CFLAGS = -Wall -Werror -Wextra -fanalyzer

# All .h Includes
INCLUDES = -IlistProcess -IappFunctions

# All cross compiler .h Includes
CRINCLUDES = -static -IlistProcess -IappFunctions

# All source files
source_files = $(wildcard *.c listProcess/*.c appFunctions/*.c)

# All directory paths are removed from source file names
file_names = $(notdir $(source_files))

# Generate open file count
appCount: $(source_files)
	$(CC) $(CFLAGS) $(INCLUDES) $(source_files) -o appCount

# Generate open file count for remote device
crossCount: $(source_files)
	$(CRC) $(CFLAGS) $(CRINCLUDES) $(source_files) -o crossCount