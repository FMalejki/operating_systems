CC = gcc
TARGET = countdown
SRC = countdown.c
CFLAGS=-g -Wall

all: $(TARGET)

$(TARGET): $(SRC)
	$(CC) $(SRC) $(CFLAGS) -o $(TARGET)

clean:
	rm -f $(TARGET)

.PHONY: all clean
