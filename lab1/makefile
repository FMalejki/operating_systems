CC=gcc
BUILD_DIR=build
CFLAGS= -Wall

.PHONY: all main_static main_shared main_dynamic_loaded clean

all:
	make main_static
	make main_shared
	make main_dynamic_loaded

main_static: main_static.c
	mkdir -p $(BUILD_DIR)
	make -C collatz collatz_static
	$(CC) $(CFLAGS) -o $(BUILD_DIR)/$@ $< collatz/build/libcollatz_static.a

main_shared: main_shared.c
	mkdir -p $(BUILD_DIR)
	make collatz_shared -C collatz
	$(CC) $(CFLAGS) -Lcollatz/build $< -lcollatz_shared -Wl,-rpath,@loader_path/../collatz/build -o $(BUILD_DIR)/$@
	
main_dynamic_loaded: main_dynamic_loaded.c
	mkdir -p $(BUILD_DIR)
	make collatz_shared -C collatz
	$(CC) $(CFLAGS) -o $(BUILD_DIR)/$@ $< -ldl

static:
	./$(BUILD_DIR)/main_static

shared:
	./$(BUILD_DIR)/main_shared

dynamic:
	./$(BUILD_DIR)/main_dynamic_loaded

clean:
	rm -rf $(BUILD_DIR)
	make -C collatz clean