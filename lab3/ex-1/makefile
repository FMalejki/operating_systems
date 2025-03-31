CC = gcc
FLAGS = -Wall
DIR = dummy_directory
BUILD_DIR = build
FILES = a.c b.c c.c

.PHONY: parent_child clean test directory

parent_child: parent_child.c
	mkdir -p $(BUILD_DIR)
	$(CC) $(CFLAGS) -o $(BUILD_DIR)/$@ $<

test: parent_child directory
	./$(BUILD_DIR)/parent_child ./$(BUILD_DIR)/$(DIR)

directory: parent_child
	mkdir -p ./$(BUILD_DIR)/$(DIR)
	@for file in $(FILES); do \
		touch ./$(BUILD_DIR)/$(DIR)/$$file; \
	done

clean:
	rm -rf $(BUILD_DIR)