CC = gcc
CFLAGS = -Wall -Wextra -g
LDFLAGS = -ltermcap

# Directory structure
SRC_DIR = src
OBJ_DIR = obj
BIN_DIR = bin
CORE_DIR = $(SRC_DIR)/core
UTILS_DIR = $(SRC_DIR)/utils
BUILTINS_DIR = $(SRC_DIR)/builtins
PARSER_DIR = $(SRC_DIR)/parser
READLINE_DIR = $(SRC_DIR)/readline

# Source files
SRCS = $(wildcard $(SRC_DIR)/*.c) \
       $(wildcard $(CORE_DIR)/*.c) \
       $(wildcard $(UTILS_DIR)/*.c) \
       $(wildcard $(BUILTINS_DIR)/*.c) \
       $(wildcard $(PARSER_DIR)/*.c) \
       $(wildcard $(READLINE_DIR)/*.c)

# Object files
OBJS = $(SRCS:$(SRC_DIR)/%.c=$(OBJ_DIR)/%.o)
TARGET = $(BIN_DIR)/minishell

# Create directories
$(shell mkdir -p $(OBJ_DIR) $(BIN_DIR) \
    $(OBJ_DIR)/core $(OBJ_DIR)/utils \
    $(OBJ_DIR)/builtins $(OBJ_DIR)/parser \
    $(OBJ_DIR)/readline)

# Default target
all: $(TARGET)

# Link object files
$(TARGET): $(OBJS)
	$(CC) $(OBJS) -o $@ $(LDFLAGS)

# Compile source files
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

# Clean build artifacts
clean:
	rm -rf $(OBJ_DIR) $(BIN_DIR)

# Run with valgrind for memory leak checking
valgrind: $(TARGET)
	valgrind --leak-check=full --show-leak-kinds=all ./$(TARGET)

.PHONY: all clean valgrind 