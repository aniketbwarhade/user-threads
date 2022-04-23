#include <sys/types.h>
#include <stdio.h>
#include <setjmp.h>

// possible state of threads
#define THREAD_RUNNING 0
#define THREAD_READY 1
#define THREAD_BLOCKED 2
#define THREAD_DEAD 3
#define THREAD_EXITED 4
typedef unsigned long thread_t;
/*Thread Control Block structure */
typedef struct tcb
{
    thread_t thread_id;
    void *stack;
    void *args;
    int state;
    void *return_val;
    void *(*start_func)(void *);
    jmp_buf buf;
    struct tcb *blocked_join; // Thread blocking on this thread
} tcb;

typedef struct thread_attr_t
{
    unsigned long stack_size; /* Stack size to be used by this thread. Default is SIGSTKSZ */
} thread_attr_t;

thread_t get_self_thread_id(void);
int add_main_thread(void);
int create_new_thread(thread_t *t, const thread_attr_t *attr, void *(*start_function)(void *), void *arg);
int init_thread(void);
