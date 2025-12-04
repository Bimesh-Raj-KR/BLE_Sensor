
# Compiler
CC = aarch64-linux-gnu-gcc

# All W Flags
CFLAGS = -Wall -Werror -Wextra -fanalyzer

# All .h Includes
INCLUDES = -static -IlistFunctions -IcheckFunctions

# All source files
source_files = $(wildcard *.c listFunctions/*.c checkFunctions/*.c)

# Generate open file list
List: $(source_files)
	$(CC) $(CFLAGS) $(INCLUDES) $(source_files) -o appList