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
#include <syscall.h>
#include "doublyll.h"

// Return thread_t of calling thread
thread_t get_self_thread_id(void)
{
    thread_t self_tid;
    self_tid = gettid();
    return self_tid;
}

// First function to be called in the main application program. which creates the main thread.
int add_main_thread(void)
{
    tcb *main;
    atexit(free_thread_list); // removing thread_details from memory before exiting,
    main = (tcb *)malloc(sizeof(tcb));
    if (main == NULL)
    {
        perror("ERROR: Unable to allocate memory for main thread.\n");
        return errno;
    }

    main->start_func = NULL;
    main->args = NULL;
    main->state = THREAD_READY;
    main->return_val = NULL;
    main->blocked_join = NULL;
    main->thread_id = get_self_thread_id();

    addthread_l(main);
    return 0;
}

int function_start(void *thread)
{
    tcb *t = (tcb *)thread;
    t->return_val = t->start_func(t->args);
    t->state = THREAD_EXITED;
    return 0;
}

int create_new_thread(thread_t *t, thread_attr_t *attr, void *(*start_func)(void *), void *args)
{
    thread_t tid;
    unsigned long stack_size;
    char *thread_child_stack; // Pointer to stack of child thread
    char *stack_top;
    tcb *child_thread;
    int ret;

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
    tid = clone(&function_start, stack_top, CLONE_VM | SIGCHLD, child_thread);
    if (tid < 0)
    {
        printf("ERROR: Clone failed unable to create the child process.\n");
        errno = EINVAL;
        return errno;
    }
    child_thread->thread_id = tid;
    // Add created thread_struct to list of thread blocks
    addthread_l(child_thread);

    (*t) = tid;
    return 0;
}

// Function to wait for a specific thread to terminate
int join_thread(thread_t thread, void **retval)
{
    thread_t t;
    tcb *calling_thread, *called_thread;
    called_thread = getthread_l(thread);
    calling_thread = getthread_l(get_self_thread_id());
    // If the thread is already dead, no need to wait. Just collect the return value and exit
    if (called_thread->state == THREAD_DEAD)
    {
        *retval = called_thread->return_val;
        return 0;
    }

    // If the thread is not dead and someone else is already waiting on it return an error

    if (called_thread->blocked_join != NULL)
        return -1;

    called_thread->blocked_join = calling_thread;
    calling_thread->state = THREAD_BLOCKED;

    t = waitpid(thread, NULL, 0);

    if (t == -1)
        perror("thread has exited\n");

    /* Target thread died, collect return value and return */
    if (retval)
    {
        *retval = called_thread->return_val;
    }
    return 0;
}

/*
 *   Logic of exit_thread code is referred from a git repo, link is pasted below:
 *   https://gitlab.com/riddhighate.07/multithreading-library.git
 */

// Function to make a thread terminate itself return value of the thread to be available to thread_join()

void exit_thread(void *ret_val)
{
    tcb *curr_thread;
    thread_t tid = get_self_thread_id();
    curr_thread = getthread_l(tid);
    thread_t main_thread_id = getpid();
    if (main_thread_id == tid)
    {
        node *temp;
        temp = thread_list->head;
        while (temp != NULL)
        {
            if (temp->thread->thread_id != main_thread_id)
            {
                int c_tid = temp->thread->thread_id;
                join_thread(c_tid, NULL);
            }
            temp = temp->next;
        }
    }

    curr_thread->return_val = ret_val;
    if (curr_thread->blocked_join != NULL)
        curr_thread->blocked_join->state = THREAD_READY;
    curr_thread->state = THREAD_DEAD;
    exit(0);
}

// Function to send signals to a specific thread All Error checking is done before
// actually raising the signal to the  specified thread and status is returned
int kill_thread(thread_t thread, int signal)
{
    if (signal == 0)
    { // if signal is 0 no signal is send.
        return 0;
    }
    if (signal < 0 || signal > 64)
    { // if signal is invalid return EINVAL
        return EINVAL;
    }
    int tid, ret_val;
    tcb *target_thread;
    int pid = get_self_thread_id();
    target_thread = getthread_l(thread);
    tid = (unsigned long)target_thread->thread_id;
    ret_val = tgkill(pid, tid, signal);
    // printf("kill returns: %d\n", ret_val);
    return ret_val;
}

// free all the threads struct from list
void free_thread_list()
{
    tcb *t;
    // free individual threads
    while (thread_list->head != NULL)
    {
        t = removethread_l();
        free(t->stack);
        free(t);
    }
    // finally free thread_list
    free(thread_list);
    printf("\nfinally clear all threads\n");
}