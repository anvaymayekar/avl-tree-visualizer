# Project settings
TARGET = AVLTreeVisualizer
SRC_DIR = src
INC_DIR = include
BUILD_DIR = build
OBJ_DIR = $(BUILD_DIR)/obj

# Source and object files
SRC = $(wildcard $(SRC_DIR)/*.c)
OBJ = $(patsubst $(SRC_DIR)/%.c,$(OBJ_DIR)/%.o,$(SRC))

# Compiler and flags
CC = gcc
CFLAGS = -std=c11 -Wall -Wextra -pedantic -I$(INC_DIR)
LDFLAGS =

# Detect platform (Windows vs others)
ifeq ($(OS),Windows_NT)
    # Link Windows GUI libraries
    LDFLAGS += -lgdi32 -luser32 -lkernel32 -mwindows
    # For console debugging instead of GUI, comment out above and uncomment below:
    # LDFLAGS += -lgdi32 -luser32 -lkernel32 -mconsole
endif

# Default target
all: $(BUILD_DIR)/$(TARGET)

# Link final executable
$(BUILD_DIR)/$(TARGET): $(OBJ)
	@mkdir -p $(BUILD_DIR)
	$(CC) $(OBJ) -o $@ $(LDFLAGS)
	@echo "Build complete → $@"

# Compile object files into build/obj/
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	@mkdir -p $(OBJ_DIR)
	$(CC) $(CFLAGS) -c $< -o $@
	@echo "Compiled $< → $@"

# Clean build files
clean:
	rm -rf $(BUILD_DIR)
	@echo "Cleaned all build artifacts."

.PHONY: all clean
