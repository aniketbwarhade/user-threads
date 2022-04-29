#include "thread.h"
typedef struct node
{
    tcb *thread;
    struct node *prev;
    struct node *next;
} node;

typedef struct queue
{
    struct node *front;
    struct node *rear;
} queue;

queue *thread_list; // Global pointer pointing to thread queue

void init_thread_l(void);
void addthread_l(tcb *thread);
tcb *removethread_l();
int is_empty();
tcb *getthread_l(thread_t thread); // Get tcb_structure from thread_t
