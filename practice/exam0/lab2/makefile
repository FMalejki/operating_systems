CC=gcc
TEST_DIR=art
OUT_DIR=output

.PHONY: flipper clean unpack

unpack:
	tar -xvzf art.tgz

flipper:
	$(CC) flipper.c -o flipper

test: flipper unpack
	./flipper $(TEST_DIR) $(OUT_DIR)

clean:
	rm -rf output flipper art