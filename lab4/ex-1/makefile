CC = gcc
CFLAGS = -Wall
TARGETS = catcher sender

all: $(TARGETS)

catcher: catcher.c
	$(CC) $(CFLAGS) -o catcher catcher.c

sender: sender.c
	$(CC) $(CFLAGS) -o sender sender.c

clean:
	rm -f $(TARGETS) catcher sender
