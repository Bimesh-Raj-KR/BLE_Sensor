
# Compiler
R_CC = gcc
T_CC = g++

# Version
ENABLE = -std=c++17

# All W Flags
CFLAGS = -Wall -Werror -Wextra -fanalyzer

# All .h Includes
INCLUDES = -IportStruct

# All linkers
LINKERS = -lgtest -lgtest_main

# Debug and optimization
OPT = -g -O0

# All source files
src_run = $(wildcard *.c portStruct/*.c)
src_test = $(wildcard *.cpp portStruct/*.c)

# Generate binaries
run: $(src_run)
	$(R_CC) $(CFLAGS) $(INCLUDES) $(src_run) -o run

test: $(src_test)
	$(T_CC) $(ENABLE) $(INCLUDES) $(src_test) $(LINKERS) -o test

clean: $(source_fileOps) $(source_fileStub)
	rm -rf run
	rm -rf test