BUILD_DIR = build
CC = gcc
FLAGS = -Wall
ARGS = none ignore handler mask


signal: signals.c
	mkdir -p $(BUILD_DIR)
	$(CC) $(FLAGS) $< -o $(BUILD_DIR)/$@

clean:
	rm -rf $(BUILD_DIR)

test: signal
	@for argument in $(ARGS); do \
		./$(BUILD_DIR)/$< $$argument; \
	done
