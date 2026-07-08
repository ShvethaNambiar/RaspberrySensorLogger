# Compiler to use
CC = gcc

# Compiler flags (-Wall shows warnings, -pthread enables multithreading)
CFLAGS = -Wall -pthread

# Libraries that must be appended at the end of the linking step
LIBS = -lpigpio -lrt

# Name of your final executable
TARGET = sensorLogger

# Automatically find all .c files in the directory
SRCS = $(wildcard ../*.c)

# Convert .c filenames to .o (object) filenames
OBJS = $(SRCS:.c=.o)

# Default rule: builds the target executable
all: $(TARGET)

# Rule to link object files into the final executable
$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJS) $(LIBS)

# Rule to compile .c files into .o files
%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

# Clean rule to delete compiled files and start fresh
clean:
	rm -f $(TARGET) $(OBJS)
