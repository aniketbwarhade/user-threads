# Multithreading-Library
Implementation of User-Level multithreading library for Unix-like POSIX compliant systems .


This include two Mappings:
1. One-one model
2. Many-one model

Data Structures:
---------------
Double ended Queue for the imeplementation of 
FIFO scheduling of threads.

1.One-One Model:
---------------
### Each user thread get mapped to its corresponding kernel thread.

Following twelve functions are implemented:
1. create_new_thread() : Clone is used.
2. join_thread()
3. exit_thread()
4. kill_thread() : All signals are handled as in pthread_kill.
5. get_self_thread_id(): Will return the thread-id of calling thread.
6. thread_spin_init();   synchronization using spinlocks
7. thread_spin_lock();
8. thread_spin_unlock();
9. thread_spin_trylock();
// synchronization using mutex
10. thread_mutex_init();
11. thread_mutex_lock();
12. thread_mutex_unlock();


The prototype of this functions is included in thread.h file.

To Run One-one model:
---------------------
1. Go to "one-one" folder.
2. Create test.c file and write your code in it.
3. Include "./thread.h" in your test.c file.
4. Run command "make".
5. Run "./test".

Test Files:
-----------
test.c : Testing file which test the all implemented functionality of One-one model.



2.Many-One Model:
---------------
### There is a mapping for many user level threads onto a single kernel thread.
### SIGVTALRM signals are used to do the scheduling of user threads .
Following twelve functions are implemented:
1. create_new_thread() 
2. join_thread()
3. exit_thread()
4. kill_thread() : Signals SIGSTOP, SIGCONT, SIGTERM, SIGHUP, SIGINT and SIGKILL are handled.
5. get_self_thread_id(): Will return the thread-id of calling thread.
6. thread_spin_init();   synchronization using spinlock
7. thread_spin_lock();
8. thread_spin_unlock();
9. thread_spin_trylock();
// synchronization using mutex
10. thread_mutex_init();
11. thread_mutex_lock();
12. thread_mutex_unlock();

The prototype of this functions is included in thread.h file.

To Run Many-one model:
----------------------
1. Go to "many-one" folder.
2. Create test.c file and write your code in it.
3. Include "./thread.h" in your test.c file.
4. Run command "make".
5. Run "./test".

Test Files:
-----------

test.c : Testing file which test the all implemented functionality of Many-one model.

