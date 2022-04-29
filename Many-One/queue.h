#include <stdio.h>
#include "thread.h"

typedef struct node
{
    tcb *list_thread;
    struct node *prev;
    struct node *next;
} node;

typedef struct queue
{
    struct node *front;
    struct node *rear;
} queue;

void enqueue(tcb *thread, queue *q);
tcb *dequeue_thread(queue *q);
int is_empty(queue *q);
tcb *getthread_q(queue *q, thread_t thread);
int threads_count(queue *q);
