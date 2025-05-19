.PHONY: all clean run

integral:
	gcc -pthread integral.c -o integral -lm

clean:
	rm -f integral

run: integral
	./integral 0.00000001 16
	./integral 0.0000001 16	
	./integral 0.000001 16	