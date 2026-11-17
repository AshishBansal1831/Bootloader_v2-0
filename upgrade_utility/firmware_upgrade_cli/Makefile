CC = g++
CFLAGS = -Wall -std=c++17

# Include directories converted into -I compiler flags
INC_DIR = \
    Inc \
    ../common_libr

INCLUDES = $(addprefix -I, $(INC_DIR))

# Source directories to scan
SRC_DIR = \
    Src \
    ../common_libr/buffering \
	../common_libr/crc \
	../common_libr/InterfaceIO \
	../common_libr/as_pkt

# Automatically find all .cpp and .c files inside the source directories
SRC_FILES = $(foreach dir, $(SRC_DIR), $(wildcard $(dir)/*.cpp) $(wildcard $(dir)/*.c))

# $(info === DEBUG: SRC_FILES are: $(SRC_FILES) ===)

# Generate object file paths from source files
OBJ_FILES = $(patsubst %.cpp, %.o, $(patsubst %.c, %.o, $(SRC_FILES)))

# Name of the final executable output
TARGET = bootloader_host_tool

all: $(TARGET)

# Link all object files into the final executable
$(TARGET): $(OBJ_FILES)
	$(CC) $(CFLAGS) $(OBJ_FILES) -o $(TARGET) $(INCLUDES)

# Compile C++ source files into object files
%.o: %.cpp
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

# Compile C source files into object files
%.o: %.c
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

# Clean up build artifacts
clean:
	rm -f $(OBJ_FILES) $(TARGET)

.PHONY: all clean