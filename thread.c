#define _GNU_SOURCE
#include <sched.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/wait.h>

#include "thread.h"

int init_thread()
{
    tcb *thread = (tcb *)calloc(1, sizeof(thread));
    thread->args = NULL;
    thread->return_val = NULL;
    thread->stack = NULL;
    thread->pid = -1;
    thread->thread_id = 0;
    thread->start_func = NULL;
    return 0;
}

int create_new_thread(thread_t *t, thread_attr_t *attr, void *(*start_func)(void *), void *args)
{
    thread_t tid;
    unsigned long stack_size;
    char *thread_child_stack; // Pointer to stack of child thread
    char *stack_top;
    tcb *child_thread;

    if (!t || !start_func)
    {
        errno = EINVAL;
        return errno;
    }

    child_thread = (tcb *)calloc(1, sizeof(tcb));
    if (child_thread == NULL)
    {
        perror("ERROR: Unable to allocate memory for tcb.\n");
        errno = ENOMEM;
        return errno;
    }
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
    child_thread->stack = thread_child_stack;

    stack_top = thread_child_stack + stack_size;

    child_thread->args = args;
    child_thread->state = THREAD_RUNNING;
    child_thread->start_func = start_func;
    child_thread->args = args;
    int (*func)(void *) = (int (*)(void *))start_func; // typecast (void *)(*start_func) to int (*func)
    tid = clone(func, stack_top, CLONE_VM | SIGCHLD, NULL);
    if (tid == EINVAL || tid == ENOMEM)
    {
        printf("Stack problem");
    }
    if (tid < 0)
    {
        printf("ERROR: Clone failed unable to create the child process.\n");
        errno = EINVAL;
        return errno;
    }
    child_thread->thread_id = tid;
    child_thread->pid = getpid();

    printf("\nParent_id : %lu \n", child_thread->pid);
    printf("\nChild_tid : %lu \n", child_thread->thread_id);
    wait(NULL);
    free(thread_child_stack);

    (*t) = tid;
    return 0;
}
