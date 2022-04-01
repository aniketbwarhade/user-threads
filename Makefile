all: threading_lib clean
threading_lib: test.o thread.o
	gcc test.o thread.o -o threading_lib

test.o: test.c
	gcc -c test.c

thread.o: thread.c thread.h
	gcc -c thread.c

clean:
	rm *.o 
