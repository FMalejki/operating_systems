CC = gcc
FLAGS = -Wall
FILE_NAME = signals
BUILD_DIR = build
ARG = none ignore handler mask


.PHONY = signals test clean

signals: $(FILE_NAME).c
	mkdir -p $(BUILD_DIR)
	$(CC) $(FLAGS) -o $(BUILD_DIR)/$@ $<

test: signals
	@for behaviour in $(ARG); do \
		./$(BUILD_DIR)/$(FILE_NAME) $$behaviour; \
	done

clean:
	rm -rf $(BUILD_DIR)
	
