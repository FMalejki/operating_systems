CC=gcc
BUILD_DIR=build
CFLAGS := -Wall
TIME := /usr/bin/time

.PHONY: all clean


all: calculate input

input: input.c
	mkdir -p $(BUILD_DIR)
	$(CC) $(CFLAGS) -o $(BUILD_DIR)/$@ $<

calculate: calculate.c
	mkdir -p $(BUILD_DIR)
	$(CC) $(CFLAGS) -o $(BUILD_DIR)/$@ $<

test_calculate: calculate 
	./$(BUILD_DIR)/calculate

test_input: input
	./$(BUILD_DIR)/input

clean:
	rm -rf $(BUILD_DIR)
	rm -rf *.fifo