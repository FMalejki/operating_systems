CC = gcc
FLAGS = -Wall
BUILD_DIR = build

all: posix

posix: posix.c
	mkdir -p $(BUILD_DIR)
	$(CC) $(FLAGS) $< -o $(BUILD_DIR)/$@

clean:
	rm -rf $(BUILD_DIR)

test_posix: posix
	./$(BUILD_DIR)/$<


