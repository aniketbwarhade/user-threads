all: threading_lib clean
threading_lib: test.o thread.o list.o
	gcc test.o thread.o list.o -o threading_lib

test.o: test.c
	gcc -c test.c

thread.o: thread.c thread.h 
	gcc -c thread.c

list.o: list.c list.h
	gcc -c list.c

clean:
	rm *.o 
