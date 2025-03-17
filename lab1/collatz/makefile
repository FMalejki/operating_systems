CC=gcc
BUILD_DIR=build
CFLAGS= -Wall

.PHONY: all, collatz_static, collatz_shared

all:
	make collatz_static
	make collatz_shared

collatz_static: collatz.c
	mkdir -p $(BUILD_DIR)
	$(CC) $(CFLAGS) -o $(BUILD_DIR)/$@ -c $<
	ar rcs $(BUILD_DIR)/lib$@.a $(BUILD_DIR)/$@

collatz_shared: collatz.c
	mkdir -p $(BUILD_DIR)
	$(CC) -Wall -fPIC -o $(BUILD_DIR)/$@.o -c $<
	$(CC) -shared -Wl,-install_name,@rpath/libcollatz_shared.so -o $(BUILD_DIR)/lib$@.so $(BUILD_DIR)/$@.o


clean:
	rm -rf $(BUILD_DIR)