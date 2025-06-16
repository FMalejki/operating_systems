all: init user printer

init: init.c
	gcc init.c -o init

user: user.c
	gcc user.c -o user

printer: printer.c
	gcc printer.c -o printer

clean:
	rm -f init user printer
