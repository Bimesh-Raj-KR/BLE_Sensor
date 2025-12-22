
# Compiler
CC = gcc

# All W Flags
CFLAGS = -Wall -Werror -Wextra -fanalyzer

#All common .h Includes
INCLUDES = -ImutexOperations -IsharedMemory

# All .h Includes for write 
INCLUDES_WRITE = -IwriteFile -IwriteFile/writeFunctions

# All .h Includes for read
INCLUDES_READ = -IreadFile -IreadFile/readFunctions

# All common source files
source_files_common = $(wildcard mutexOperations/*.c sharedMemory/*.c)

#All source files for write
source_files_write = $(wildcard writeFile/*.c writeFile/writeFunctions/*.c)

# All source files for read
source_files_read = $(wildcard readFile/*.c readFile/readFunctions/*.c)

# Generate app write
write: $(source_files_common) $(source_files_write)
	$(CC) $(CFLAGS) $(INCLUDES) $(INCLUDES_WRITE) $(source_files_common) \
	$(source_files_write) -o appWrite

# Generate app read
read: $(source_files_common) $(source_files_read)
	$(CC) $(CFLAGS) $(INCLUDES) $(INCLUDES_READ) $(source_files_common) \
	$(source_files_read) -o appRead

