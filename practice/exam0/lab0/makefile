CC = gcc
FLAGS = -Wall
BUILD_DIR = build

.PHONY: all countdown clean test

all: countdown

countdown: countdown.c
	mkdir -p $(BUILD_DIR)
	$(CC) $(FLAGS) -o $(BUILD_DIR)/$@ $<

clean:
	rm -rf $(BUILD_DIR)

test: countdown
	./$(BUILD_DIR)/$<