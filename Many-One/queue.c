#include <stdlib.h>
#include "queue.h"
void enqueue(tcb *thread, queue *q)
{
    node *t = (node *)malloc(sizeof(node));
    t->list_thread = thread;
    t->next = NULL;
    if (is_empty(q))
    {
        q->front = q->rear = t;
    }
    else
    {
        t->prev = q->rear;
        q->rear->next = t;
        q->rear = t;
    }
    return;
}

tcb *dequeue_thread(queue *q)
{
    if (is_empty(q))
    {
        return NULL;
    }
    tcb *ret = (tcb *)malloc(sizeof(tcb));
    node *t = (node *)malloc(sizeof(node *));
    t = q->front;
    ret = t->list_thread;
    q->front = q->front->next;
    t->next = NULL;
    t->prev = NULL;

    if (q->front == NULL)
    {
        q->rear = NULL;
    }
    else
    {
        q->front->prev = NULL;
    }

    return (ret);
}

int is_empty(queue *q)
{
    return (!q->front && !q->rear);
}

tcb *getthread_l(queue *q, thread_t thread)
{
    node *t = (node *)malloc(sizeof(node));
    t = q->front;
    int pos = 0;
    if (t == NULL)
    {
        printf("queue is null\n");
    }
    while (((t)->list_thread->thread_id != thread) && t->next != NULL)
    {
        pos++;
        t = t->next;
    }

    if ((t)->list_thread->thread_id != thread)
    {
        return NULL;
    }
    return t->list_thread;
}

int threads_count(queue *q)
{
    node *t = (node *)malloc(sizeof(node));
    t = q->front;
    int count = 0;
    while (t != NULL)
    {
        count++;
        t = t->next;
    }
    return count;
}