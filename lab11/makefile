CC = gcc
FLAGS = -Wall
BUILD_DIR = build
FILE_NAME_1 = client
FILE_NAME_2 = server
ARGS = 1 2 4

.PHONY: build clean test

clean: 
	rm -rf $(BUILD_DIR)

$(FILE_NAME_1):
	mkdir -p $(BUILD_DIR)
	$(CC) $(FLAGS) $(FILE_NAME_1).c -o $(BUILD_DIR)/$(FILE_NAME_1)

$(FILE_NAME_2):
	mkdir -p $(BUILD_DIR)
	$(CC) $(FLAGS) $(FILE_NAME_2).c -o $(BUILD_DIR)/$(FILE_NAME_2)	

test:
	./$(BUILD_DIR)/$(FILE_NAME) $(ARGS)