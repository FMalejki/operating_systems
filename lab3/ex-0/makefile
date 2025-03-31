CC = gcc
BUILD_DIR = build
CFLAGS = -Wall
ARG = 10

.PHONY: processes clean

processes: processes.c
	mkdir -p $(BUILD_DIR)

	$(CC) $(CFLAGS) -o $(BUILD_DIR)/$@ $<
test: processes
	./$(BUILD_DIR)/processes $(ARG)

clean:
	rm -rf $(BUILD_DIR)