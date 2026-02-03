CC = gcc
CFLAGS = -Wall -Wextra -g -MMD -MP -Iinclude -Isrc

SRC_DIR = src
BUILD_DIR = build
BIN_DIR = bin
TARGET = $(BIN_DIR)/main
COMPILE_DB = $(BUILD_DIR)/compile_commands.json

SRCS := $(shell find $(SRC_DIR) -name '*.c' -type f)
OBJS := $(patsubst $(SRC_DIR)/%.c,$(BUILD_DIR)/%.o,$(SRCS))
DEPS := $(OBJS:.o=.d)

all: $(TARGET)
	@echo "Compilation completed: $(TARGET)"

$(TARGET): $(OBJS)
	@mkdir -p $(BIN_DIR)
	$(CC) $(CFLAGS) -o $@ $^

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

-include $(DEPS)

run:
	./bin/main

# 生成编译数据库到 build/
compile_db:
	@mkdir -p $(BUILD_DIR)
	bear --output $(COMPILE_DB) -- make clean all
	@echo "Compilation database generated at: $(COMPILE_DB)"

clean:
	@if [ -d "$(BUILD_DIR)" ]; then \
		find "$(BUILD_DIR)" -name '*.o' -delete; \
		find "$(BUILD_DIR)" -name '*.d' -delete; \
		find "$(BUILD_DIR)" -name '*.so' -delete; \
		find "$(BUILD_DIR)" -name '*.a' -delete; \
	fi
	@if [ -d "$(BIN_DIR)" ]; then \
		find "$(BIN_DIR)" -type f -executable -delete; \
		find "$(BIN_DIR)" -name '*.so' -delete; \
		find "$(BIN_DIR)" -name '*.a' -delete; \
	fi
	@echo "Clean completed"

.PHONY: all clean compile_db run