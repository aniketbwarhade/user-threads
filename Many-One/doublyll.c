#include <stdio.h>
#include <stdlib.h>
#include "doublyll.h"

void init_thread_l()
{
    thread_list = (list *)malloc(sizeof(list));
    thread_list->head = NULL;
    thread_list->tail = NULL;
}

void addthread_l(tcb *thread)
{
    node *temp = (node *)malloc(sizeof(node));
    temp->thread = thread;
    temp->next = NULL;
    temp->prev = NULL;

    if (thread_list->head == NULL && thread_list->tail == NULL)
    {
        thread_list->head = thread_list->tail = temp;
    }
    else
    {
        temp->prev = thread_list->tail;
        thread_list->tail->next = temp;
        thread_list->tail = temp;
    }
    return;
}

tcb *removethread_l()
{
    if (is_empty())
    {
        return NULL;
    }
    tcb *ret;
    node *temp;
    temp = thread_list->head;
    ret = temp->thread;
    thread_list->head = temp->next;
    temp->next = NULL;
    temp->prev = NULL;
    free(temp);

    if (thread_list->head == NULL)
    {
        thread_list->tail = NULL;
    }
    else
    {
        thread_list->head->prev = NULL;
    }

    return ret;
}

int is_empty()
{
    return (!thread_list->head && !thread_list->tail);
}

tcb *getthread_l(thread_t thread)
{
    node *temp = (node *)malloc(sizeof(node));
    temp = thread_list->head;
    while (((temp)->thread->thread_id != thread) && temp->next != NULL)
    {
        temp = temp->next;
    }

    if ((temp)->thread->thread_id != thread)
    {
        return NULL;
    }
    return temp->thread;
}
