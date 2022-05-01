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
#include "queue.h"
#include "locks.h"

spinlock *lock;
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
    thread_spin_init(lock);
    tcb *main;
    main = (tcb *)malloc(sizeof(tcb));
    if (main == NULL)
    {
        perror("ERROR: Unable to allocate memory for main thread.\n");
        return errno;
    }

    main->start_func = NULL;
    main->args = NULL;
    main->state = READY;
    main->return_val = NULL;
    main->blocked_join = NULL;
    main->thread_id = get_self_thread_id();

    addthread_q(main);
    return 0;
}

int start_routine(void *thread)
{
    tcb *t = (tcb *)thread;
    t->return_val = t->start_func(t->args);
    t->state = DEAD;
    return 0;
}

int create_new_thread(thread_t *t, thread_attr_t *attr, void *(*start_func)(void *), void *args)
{
    thread_spin_lock(lock);
    thread_t thread_id;
    unsigned long stack_size;
    char *thread_child_stack; // Pointer to stack of child thread
    char *stack_top;
    tcb *child_thread;
    int ret;

    if (!t || !start_func)
    {
        thread_spin_unlock(lock);
        errno = EINVAL;
        return errno;
    }

    child_thread = (tcb *)calloc(1, sizeof(tcb));
    if (child_thread == NULL)
    {
        thread_spin_unlock(lock);
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
        thread_spin_unlock(lock);
        printf("ERROR: Unable to allocate memory for stack.\n");
        exit(EXIT_FAILURE);
    }
    child_thread->stack = thread_child_stack;

    stack_top = thread_child_stack + stack_size;

    child_thread->args = args;
    child_thread->state = RUNNING;
    child_thread->start_func = start_func;
    thread_id = clone(&start_routine, stack_top, CLONE_VM | SIGCHLD | CLONE_FS | CLONE_FILES | CLONE_SIGHAND, child_thread);
    if (thread_id < 0)
    {
        thread_spin_unlock(lock);
        printf("ERROR: Clone failed unable to create the child process.\n");
        errno = EINVAL;
        return errno;
    }
    child_thread->thread_id = thread_id;
    // Add created thread_struct to queue of thread blocks
    addthread_q(child_thread);

    (*t) = thread_id;
    thread_spin_unlock(lock);
    return 0;
}

// Function to wait for a specific thread to terminate
int join_thread(thread_t thread, void **retval)
{
    thread_spin_lock(lock);
    thread_t t;
    tcb *calling_thread, *called_thread;
    called_thread = getthread_q(thread);
    calling_thread = getthread_q(get_self_thread_id());
    thread_spin_unlock(lock);
    // If the thread is already dead, no need to wait. Just collect the return value and exit
    if (called_thread->state == DEAD)
    {
        if (retval)
            *retval = called_thread->return_val;
        return 0;
    }

    // If the thread is not dead and someone else is already waiting on it return an error

    if (called_thread->blocked_join != NULL)
        return -1;

    called_thread->blocked_join = calling_thread;
    calling_thread->state = BLOCKED;

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

// Function to make a thread terminate itself return value of the thread to be available to thread_join()

void exit_thread(void *ret_val)
{
    tcb *curr_thread;
    thread_spin_lock(lock);
    thread_t thread_id = get_self_thread_id();
    curr_thread = getthread_q(thread_id);

    curr_thread->return_val = ret_val;
    if (curr_thread->blocked_join != NULL)
        curr_thread->blocked_join->state = READY;
    curr_thread->state = DEAD;
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
    int thread_id, ret_val;
    tcb *target_thread;
    int pid = get_self_thread_id();
    target_thread = getthread_q(thread);
    thread_id = (unsigned long)target_thread->thread_id;
    ret_val = kill(thread_id, signal);
    // printf("kill returns: %d\n", ret_val);
    return ret_val;
}

// free all the threads struct from queue
void free_thread_list()
{
    thread_spin_lock(lock);
    tcb *t;
    // free individual threads
    while (thread_list->front != NULL)
    {
        t = removethread_q();
        free(t->stack);
        free(t);
    }
    // finally free thread_list
    free(thread_list);
    thread_spin_unlock(lock);
    // printf("\nfinally clear all threads\n");
}