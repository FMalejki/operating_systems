CC = gcc
FLAGS = -Wall
DIR = dummy_directory
BUILD_DIR = build
FILES = a.c b.c c.c

.PHONY: process clean test directory

process: process.c
	mkdir -p $(BUILD_DIR)
	$(CC) $(CFLAGS) -o $(BUILD_DIR)/$@ $<

test: process directory
	./$(BUILD_DIR)/process ./$(BUILD_DIR)/$(DIR)

directory: process
	mkdir -p ./$(BUILD_DIR)/$(DIR)
	@for file in $(FILES); do \
		touch ./$(BUILD_DIR)/$(DIR)/$$file; \
	done

clean:
	rm -rf $(BUILD_DIR)