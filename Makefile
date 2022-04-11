all: threading_lib clean
threading_lib: test.o thread.o doublyll.o
	gcc test.o thread.o doublyll.o -o threading_lib

test.o: test.c
	gcc -c test.c

thread.o: thread.c thread.h 
	gcc -c thread.c

doublyll.o: doublyll.c doublyll.h
	gcc -c doublyll.c

clean:
	rm *.o 
