#define _GNU_SOURCE
#include <sched.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <errno.h>
#include "thread.h"
int create_new_thread(thread_t *t, thread_attr_t *attr, void *(*start_func)(void *), void *args)
{
    thread_t tid;
    unsigned long stack_size;
    char *thread_child_stack; // Pointer to stack of child thread
    char *stack_top;
    tcb *child_thread;

    // Create stack for child thread
    if (attr)
    {
        stack_size = attr->stack_size;
    }
    else
    {
        stack_size = SIGSTKSZ; /* System default stack size.  */
    }

    thread_child_stack = (char *)malloc(stack_size);

    if (thread_child_stack == NULL)
    {
        printf("ERROR: Unable to allocate memory for stack.\n");
        exit(EXIT_FAILURE);
    }

    stack_top = thread_child_stack + stack_size;

    child_thread = (tcb *)malloc(sizeof(tcb));
    if (child_thread == NULL)
    {
        perror("ERROR: Unable to allocate memory for tcb.\n");
        exit(EXIT_FAILURE);
    }
    child_thread->args = args;

    tid = clone((int (*)(void *))start_func, stack_top, SIGCHLD, NULL);
    (*t) = tid;
    // printf("%lu\n", tid);
    if (tid < 0)
    {
        printf("ERROR: Unable to create the child process.\n");
        exit(EXIT_FAILURE);
    }

    child_thread->thread_id = tid;
    child_thread->pid = getpid();

    return 0;
}

void *func(void *arg)
{
    printf("\n this code will run inside Child process.\n");
    return NULL;
}

int main()
{
    thread_t tid;
    printf("%lu\n", tid);
    create_new_thread(&tid, NULL, func, NULL);
    printf("%lu\n", tid);

    return 0;
}