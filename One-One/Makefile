all: threading_lib clean
threading_lib:  spinlock_count.o mutexlock_count.o test.o locks.o thread.o queue.o
	gcc  locks.o test.o thread.o queue.o -o f_testing
	./f_testing
	gcc  locks.o mutexlock_count.o thread.o queue.o -o m_testing
	./m_testing
	

test.o: test.c
	gcc -c test.c

mutexlock_count.o: mutexlock_count.c
	gcc -c mutexlock_count.c

spinlock_count.o: spinlock_count.c
	gcc -c spinlock_count.c

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
