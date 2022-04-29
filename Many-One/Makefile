all: mthreading_lib
mthreading_lib: test.o thread.o  queue.o
	gcc test.o thread.o queue.o -o mthreading_lib

test.o: test.c
	gcc -c test.c
	

thread.o: thread.c thread.h 
	gcc -c thread.c

queue.o: queue.c queue.h
	gcc -c queue.c


