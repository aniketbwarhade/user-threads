#include <stdio.h>
#include <stdlib.h>
#include "queue.h"

void init_thread_q()
{
    thread_list = (queue *)malloc(sizeof(queue));
    thread_list->front = NULL;
    thread_list->rear = NULL;
}

void addthread_q(tcb *thread)
{
    node *t = (node *)malloc(sizeof(node));
    t->thread = thread;
    t->next = NULL;
    t->prev = NULL;

    if (thread_list->front == NULL && thread_list->rear == NULL)
    {
        thread_list->front = thread_list->rear = t;
    }
    else
    {
        t->prev = thread_list->rear;
        thread_list->rear->next = t;
        thread_list->rear = t;
    }
    return;
}

tcb *removethread_q()
{
    if (is_empty())
    {
        return NULL;
    }
    tcb *ret;
    node *t;
    t = thread_list->front;
    ret = t->thread;
    thread_list->front = t->next;
    t->next = NULL;
    t->prev = NULL;
    free(t);

    if (thread_list->front == NULL)
    {
        thread_list->rear = NULL;
    }
    else
    {
        thread_list->front->prev = NULL;
    }

    return ret;
}

int is_empty()
{
    return (!thread_list->front && !thread_list->rear);
}

tcb *getthread_q(thread_t thread)
{
    node *t = (node *)malloc(sizeof(node));
    t = thread_list->front;
    while (((t)->thread->thread_id != thread) && t->next != NULL)
    {
        t = t->next;
    }

    if ((t)->thread->thread_id != thread)
    {
        return NULL;
    }
    return t->thread;
}
