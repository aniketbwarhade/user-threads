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

void init_thread_q(void);
void addthread_q(tcb *thread);
tcb *removethread_q();
int is_empty();
tcb *getthread_q(thread_t thread); // Get tcb_structure from thread_t
