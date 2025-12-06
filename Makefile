# Compiler
CC = gcc
CFLAGS = -Wall -g -pthread -I./include

# Directories
SRC_DIR = src
BUILD_DIR = build
BIN_DIR = bin
INCLUDE_DIR = include

# Target
TARGET = $(BIN_DIR)/lyrics_player

# Source files
SRCS = $(wildcard $(SRC_DIR)/*.c)
# Object files
OBJS = $(patsubst $(SRC_DIR)/%.c, $(BUILD_DIR)/%.o, $(SRCS))
# Dependencies
DEPS = $(wildcard $(INCLUDE_DIR)/*.h)

# Default target
all: $(TARGET)

# Link object files to create executable
$(TARGET): $(OBJS) | $(BIN_DIR)
	$(CC) $(CFLAGS) -o $@ $(OBJS)

# Compile source files to object files
$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c $(DEPS) | $(BUILD_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

# Create directories if they don't exist
$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

$(BIN_DIR):
	mkdir -p $(BIN_DIR)

# Clean up build files
clean:
	rm -rf $(BUILD_DIR) $(BIN_DIR)

# Run the program (example usage)
run: $(TARGET)
	./$(TARGET) /test_data/song.mp3 /test_data/lyrics.lrc

# Debug target
debug: CFLAGS += -DDEBUG -O0
debug: clean $(TARGET)

# Release target
release: CFLAGS += -O3 -DNDEBUG
release: clean $(TARGET)

# Show help
help:
	@echo "Available targets:"
	@echo "  all      - Build the program (default)"
	@echo "  clean    - Remove build files"
	@echo "  run      - Build and run with example files"
	@echo "  debug    - Build with debug flags"
	@echo "  release  - Build with optimization flags"
	@echo "  help     - Show this help message"

.PHONY: all clean run debug release help