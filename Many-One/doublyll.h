#include <stdio.h>
#include "thread.h"

typedef struct node
{
    tcb *list_thread;
    struct node *prev;
    struct node *next;
} node;

typedef struct list
{
    struct node *head;
    struct node *tail;
} list;

void enqueue(tcb *thread, list *l);
tcb *dequeue_thread(list *l);
int is_empty(list *l);
tcb *get_from_list(list *l, thread_t thread);
int threads_count(list *l);
