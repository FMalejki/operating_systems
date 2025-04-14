CC = gcc
FLAGS = -Wall 
BUILD_DIR = build
ARG = 19 100

.PHONY: clean all test collatz static

collatz: collatz.c
	mkdir -p $(BUILD_DIR)
	$(CC) -c -o $(BUILD_DIR)/$@.o $<

collatz_lib: collatz
	ar rcs $(BUILD_DIR)/lib_collatz.a $(BUILD_DIR)/$<.o

main: main.c
	mkdir -p $(BUILD_DIR)
	$(CC) -c -o $(BUILD_DIR)/$@.o $<

main_static: main collatz_lib
	$(CC) $(BUILD_DIR)/lib_collatz.a $(BUILD_DIR)/$<.o -o $(BUILD_DIR)/$@

test_static: main_static
	./$(BUILD_DIR)/main_static $(ARG)

collatz_shared_lib: collatz
	$(CC) -fPIC -c -o $(BUILD_DIR)/collatz.o collatz.c
	$(CC) -shared -Wl,-install_name,@rpath/libcollatz_shared_lib.so -o $(BUILD_DIR)/lib$@.so $(BUILD_DIR)/$@.o

main_shared: main.c main collatz_shared_lib
	$(CC) $(CFLAGS) -Lbuild $< -lcollatz_shared_lib -Wl,-rpath,@loader_path/../build -o $(BUILD_DIR)/$@

main_shared_test: main_shared
	./$(BUILD_DIR)/main_shared $(ARG)

test: main_shared_test test_static main_dynamic_test

main_dynamic: main_dynamic.c
	$(CC) $< -o $(BUILD_DIR)/$@

main_dynamic_test: main_dynamic collatz_shared_lib
	./$(BUILD_DIR)/$< $(ARG)
