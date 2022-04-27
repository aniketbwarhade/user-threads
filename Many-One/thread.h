#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <ucontext.h>

#define RUNNING 0
#define READY 1
#define BLOCKED 2
#define DEAD 3
#define EXITED 4
#define STACK_SIZE 1024 * 64
#define ALARM 2000

typedef unsigned long thread_t;

typedef struct tcb
{
    thread_t thread_id;
    void *stack;
    size_t stack_size;
    void *ret_val;
    struct tcb *next;
    void *(*thread_start)(void *);
    int state;
    void *args;
    ucontext_t *t_context;
} tcb;

typedef struct thread_attr_t
{
    unsigned long stack_size; // Stack size to be used by this thread
} thread_attr_t;

int mythread_init(void);
void *allocate_stack(size_t size);
int create_new_thread(thread_t *thread, thread_attr_t *attr, void *(*thread_start)(void *), void *args);
tcb *get_cthread(void);
void strt_timer(struct itimerval *timer);
void stp_timer(struct itimerval *timer);