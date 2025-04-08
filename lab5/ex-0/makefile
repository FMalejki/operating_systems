CC = gcc
FLAGS = -Wall
BUILD_DIR = build
ARG = 0.000000001 4

.PHONY: all integral test clean

all: integral

integral: integral.c
	mkdir -p $(BUILD_DIR)
	$(CC) $(FLAGS) -o $(BUILD_DIR)/$@ $<

test:
	./$(BUILD_DIR)/integral $(ARG)

clean:
	rm -rf $(BUILD_DIR)
