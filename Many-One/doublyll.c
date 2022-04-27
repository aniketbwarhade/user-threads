#include <stdlib.h>
#include "doublyll.h"
void enqueue(tcb *thread, list *l)
{
    node *temp = (node *)malloc(sizeof(node));
    temp->list_thread = thread;
    temp->next = NULL;
    if (is_empty(l))
    {
        l->head = l->tail = temp;
    }
    else
    {
        temp->prev = l->tail;
        l->tail->next = temp;
        l->tail = temp;
    }
    return;
}

tcb *dequeue_thread(list *l)
{
    if (is_empty(l))
    {
        return NULL;
    }
    tcb *ret = (tcb *)malloc(sizeof(tcb));
    node *temp = (node *)malloc(sizeof(node *));
    temp = l->head;
    ret = temp->list_thread;
    l->head = l->head->next;
    temp->next = NULL;
    temp->prev = NULL;

    if (l->head == NULL)
    {
        l->tail = NULL;
    }
    else
    {
        l->head->prev = NULL;
    }

    return (ret);
}

int is_empty(list *l)
{
    return (!l->head && !l->tail);
}

tcb *get_from_list(list *l, thread_t thread)
{
    node *temp = (node *)malloc(sizeof(node));
    temp = l->head;
    int pos = 0;
    if (temp == NULL)
    {
        printf("List is null\n");
    }
    while (((temp)->list_thread->thread_id != thread) && temp->next != NULL)
    {
        pos++;
        temp = temp->next;
    }

    if ((temp)->list_thread->thread_id != thread)
    {
        return NULL;
    }
    return temp->list_thread;
}

int threads_count(list *l)
{
    node *temp = (node *)malloc(sizeof(node));
    temp = l->head;
    int count = 0;
    while (temp != NULL)
    {
        count++;
        temp = temp->next;
    }
    return count;
}