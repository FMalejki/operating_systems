CC = gcc
FLAGS = -Wall
BUILD_DIR = build

all: server client

server: server.c
	rm -rf /dev/mqueue/*
	mkdir -p $(BUILD_DIR)
	$(CC) $(FLAGS) $< -o $(BUILD_DIR)/$@

client: client.c
	mkdir -p $(BUILD_DIR)
	$(CC) $(FLAGS) $< -o $(BUILD_DIR)/$@

clean:
	rm -rf $(BUILD_DIR)
	rm -f /dev/mqueue/*

test_server: server
	./$(BUILD_DIR)/$<

test_client: client
	./$(BUILD_DIR)/$<


