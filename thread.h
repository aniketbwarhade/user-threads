#include <sys/types.h>
#include <stdio.h>
typedef unsigned long thread_t;
/*Thread Control Block structure */
typedef struct tcb
{
    thread_t thread_id;
    thread_t pid;
    void *stack;
    void *args;
    int state;
    void *return_val;
    void *(*start_func)(void *);
    struct tcb *blocked_join; // Thread blocking on this thread
} tcb;

// possible state of threads
enum
{
    THREAD_RUNNING,
    THREAD_READY,
    THREAD_BLOCKED,
    THREAD_DEAD
};

typedef struct thread_attr_t
{
    unsigned long stack_size; /* Stack size to be used by this thread. Default is SIGSTKSZ */
} thread_attr_t;
int add_main_thread(void);
int create_new_thread(thread_t *t, thread_attr_t *attr, void *(*start_func)(void *), void *args);
int init_thread(void);
int join_thread(thread_t thread, void **retval);
