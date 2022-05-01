#include <sys/types.h>
#include <stdio.h>

// possible state of threads
#define RUNNING 0
#define READY 1
#define BLOCKED 2
#define DEAD 3

// color for testcase
#define RED "\033[1;31m"
#define RESET "\033[0m"
#define GREEN "\e[0;32m"
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
    struct tcb *blocked_join; // Thread blocking on this thread
} tcb;

typedef struct thread_attr_t
{
    unsigned long stack_size; /* Stack size to be used by this thread. Default is SIGSTKSZ */
} thread_attr_t;
thread_t get_self_thread_id(void);
int add_main_thread(void);
int create_new_thread(thread_t *t, thread_attr_t *attr, void *(*start_func)(void *), void *args);
int init_thread(void);
int join_thread(thread_t thread, void **retval);
void exit_thread(void *ret_val);
int kill_thread(thread_t thread, int signal);
void free_thread_list();
