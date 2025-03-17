collatz_static:
	gcc -c collatz.c
	ar rcs libcollatz.a collatz.o
	gcc -c main.c
	gcc main.o libcollatz.a –o main

run: 
	./main

clean:
	rm -f libcollatz.a
	rm -f collatz.o
	rm -f main.o
	rm -f main