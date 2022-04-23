#include "thread.h"
typedef struct node
{
    tcb *thread;
    struct node *prev;
    struct node *next;
} node;

typedef struct list
{
    struct node *head;
    struct node *tail;
} list;

static list *thread_list; // Global pointer pointing to thread list
static tcb *curr_running;
void init_thread_l(void);
void addthread_l(tcb *thread);
tcb *removethread_l();
int is_empty();
tcb *getthread_l(thread_t thread); // Get tcb_structure from thread_t
