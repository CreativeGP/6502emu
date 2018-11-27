6502: main.c
	gcc main.c -o 6502 -O3 -fomit-frame-pointer

run: 6502
	./6502

clean:
	rm -f 6502 *.o *~ tmp*
