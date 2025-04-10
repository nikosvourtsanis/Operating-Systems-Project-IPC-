# Makefile for POSIX IPC 

# Compiler and flags
CC = gcc
CFLAGS = -Wall -Wextra -std=c11 -pthread -lrt

# Project files
TARGET = ipc
SRCS = ipc.c child.c empty_resources.c
HEADERS = sharedmem.h empty_resources.h child.h
OBJS = $(SRCS:.c=.o)

# Default target
all: $(TARGET)

# Build the main executable
$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJS)

# Compile source files into object files
%.o: %.c $(HEADERS)
	$(CC) $(CFLAGS) -c $< -o $@

# Clean up build files
clean:
	rm -f $(OBJS) $(TARGET)

# Phony targets
.PHONY: all clean
