#include <sys/types.h>
#include <stdio.h>
typedef unsigned long thread_t;
typedef struct tcb
{
    thread_t thread_id;
    thread_t pid;
    void *stack;
    void *args;
} tcb;

typedef struct thread_attr_t
{
    unsigned long stack_size; /* Stack size to be used by this thread. Default is SIGSTKSZ */
} thread_attr_t;

int create_new_thread(thread_t *t, thread_attr_t *attr, void *(*start_func)(void *), void *args);