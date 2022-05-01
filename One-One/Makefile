all: threading_lib 
threading_lib:  count.o locks.o thread.o queue.o
	gcc  locks.o count.o thread.o queue.o -o threading_lib

test.o: test.c
	gcc -c test.c

count.o: count.c
	gcc -c count.c

testing.o: testing.c
	gcc -c testing.c

locks.o: locks.c locks.h
	gcc -c locks.c 

thread.o: thread.c thread.h 
	gcc -c thread.c

queue.o: queue.c queue.h
	gcc -c queue.c

clean:
	rm *.o 
