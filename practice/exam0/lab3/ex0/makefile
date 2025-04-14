CC = gcc
BUILD_DIR = build
CFLAGS = -Wall
ARG = 10

.PHONY: process clean

process: process.c
	mkdir -p $(BUILD_DIR)

	$(CC) $(CFLAGS) -o $(BUILD_DIR)/$@ $<
test: process
	./$(BUILD_DIR)/process $(ARG)

clean:
	rm -rf $(BUILD_DIR)