# Makefile located in build/ directory

# Compiler
CC = gcc

# Compiler Flags
CFLAGS = -I../include -Wall

# Linker Flags
LDFLAGS = -lX11 -lm

# Directories
SRC_DIR = ../src
OBJ_DIR = ../Obj
BUILD_DIR = .
INCLUDE_DIR = ../include

# Source Files
SRC = $(SRC_DIR)/main.c $(SRC_DIR)/brightness.c

# Object Files
OBJ = $(patsubst $(SRC_DIR)/%.c, $(OBJ_DIR)/%.o, $(SRC))

# Target Binary
TARGET = brightness

# Installation Directory
PREFIX ?= /usr/local
BIN_DIR = $(PREFIX)/bin

# Default Target
all: $(TARGET)

# Link Object Files to Create Binary
$(TARGET): $(OBJ)
	$(CC) -o $(BUILD_DIR)/$@ $^ $(LDFLAGS)

# Compile Source Files to Object Files
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c | $(OBJ_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

# Create Object Directory if it Doesn't Exist
$(OBJ_DIR):
	mkdir -p $(OBJ_DIR)

# Install the Binary to /usr/local/bin/
install: $(TARGET)
	@echo "Installing $(TARGET) to $(BIN_DIR)/$(TARGET)"
	mkdir -p $(BIN_DIR)
	install -m 755 $(BUILD_DIR)/$(TARGET) $(BIN_DIR)/$(TARGET)

# Uninstall the Binary from /usr/local/bin/
uninstall:
	@echo "Uninstalling $(TARGET) from $(BIN_DIR)/$(TARGET)"
	rm -f $(BIN_DIR)/$(TARGET)

# Clean Build Artifacts
clean:
	rm -f $(OBJ) $(BUILD_DIR)/$(TARGET)

# Phony Targets
.PHONY: all clean install uninstall
