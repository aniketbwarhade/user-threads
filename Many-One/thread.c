#define _GNU_SOURCE
#include <sys/types.h>
#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <sys/syscall.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/time.h>
#include <sched.h>
#include <unistd.h>
#include <signal.h>
#include <stdint.h>
#include <asm/prctl.h>
#include <sys/prctl.h>
#include <linux/futex.h>
#include <ucontext.h>
#include <sys/mman.h>
#include "queue.h"

int tid_count = 0;
queue *ready_queue;
struct itimerval timer;
tcb *curr_thread;
ucontext_t *ucp;

void strt_timer(struct itimerval *timer)
{
    timer->it_interval.tv_sec = 0;
    timer->it_interval.tv_usec = ALARM;
    timer->it_value.tv_sec = 0;
    timer->it_value.tv_usec = ALARM;
    setitimer(ITIMER_REAL, timer, 0);
}

void stp_timer(struct itimerval *timer)
{
    timer->it_interval.tv_sec = 0;
    timer->it_interval.tv_usec = 0;
    timer->it_value.tv_sec = 0;
    timer->it_value.tv_usec = 0;
    setitimer(ITIMER_REAL, timer, 0);
}

// Function to launch the start routine
static void start_routine(void)
{
    tcb *thread;
    thread = get_cthread();
    thread->ret_val = thread->start_func(thread->args);
    thread->state = EXITED;
    setcontext(ucp);
}

/*
 *   Logic of scheduler code is referred from a git repo, link is pasted below:
 *   http://nitish712.blogspot.com/2012/10/thread-library-using-context-switching.html
 */

// Whenever sigalrm is raised scheduler is called
void scheduler()
{
    // printf("HII");
    stp_timer(&timer);
    int c = threads_count(ready_queue);
    void *prev_thread;
    prev_thread = get_cthread();
    while (c > 0)
    {
        curr_thread = dequeue_thread(ready_queue);

        if (curr_thread->state == READY)
        {
            // as glibc doesn't support arch_prctl so we have to the corresponding syscall
            syscall(SYS_arch_prctl, ARCH_SET_FS, curr_thread); // To access thread specific memory (TLS) in fs register
            strt_timer(&timer);
            ucp = (ucontext_t *)malloc(sizeof(ucontext_t));
            ucp->uc_link = 0;
            ucp->uc_flags = 0;
            ucp->uc_stack.ss_size = STACK_SIZE;
            ucp->uc_stack.ss_sp = allocate_stack(STACK_SIZE);
            getcontext(ucp);
            curr_thread->t_context->uc_link = ucp;
            swapcontext(ucp, curr_thread->t_context);
            stp_timer(&timer);
            syscall(SYS_arch_prctl, ARCH_SET_FS, prev_thread);
        }
        else
        {
            return;
        }
        c--;
    }

    strt_timer(&timer);
}

// Function to get the current thread
tcb *get_cthread(void)
{
    long address;
    syscall(SYS_arch_prctl, ARCH_GET_FS, &address);
    return (tcb *)address;
}

// First function to be called in the main application program. This creates and initialize the main thread.
// Timer is started for raising sigvtalrm signal which further will call scheduler
int add_main_thread(void)
{
    curr_thread = (tcb *)malloc(sizeof(tcb));
    curr_thread->args = NULL;
    curr_thread->ret_val = NULL;
    curr_thread->start_func = NULL;
    curr_thread->stack_size = 0;
    curr_thread->thread_id = tid_count++;
    curr_thread->t_context = (ucontext_t *)malloc(sizeof(ucontext_t));
    getcontext(curr_thread->t_context);
    ready_queue = (queue *)malloc(sizeof(queue));
    ready_queue->front = ready_queue->rear = NULL;

    /* Set up SIGALRM signal handler */
    struct sigaction action;
    sigset_t mask;
    sigfillset(&mask);
    action.sa_handler = scheduler;
    action.sa_mask = mask;
    action.sa_flags = 0;
    sigaction(SIGALRM, &action, NULL);

    strt_timer(&timer);
    return 0;
}
void *allocate_stack(size_t size)
{
    void *stack = NULL;
    stack = mmap(NULL, size, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS | MAP_STACK, -1, 0);
    if (stack == MAP_FAILED)
    {
        perror("Stack Allocation");
        return NULL;
    }
    return stack;
}

// Function for creating the thread
int create_new_thread(thread_t *t, thread_attr_t *attr, void *(*start_func)(void *), void *args)
{

    if (!t || !start_func)
    {
        return EINVAL;
    }

    tcb *new_thread = (tcb *)malloc(sizeof(tcb));
    if (!new_thread)
    {
        perror("ERROR: Unable to allocate memory for tcb.\n");
        return ENOMEM;
    }

    stp_timer(&timer);

    new_thread->thread_id = tid_count++;
    new_thread->state = RUNNING;
    new_thread->start_func = start_func;
    new_thread->args = args;
    if (attr)
    {
        new_thread->stack_size = attr->stack_size;
    }
    else
    {
        new_thread->stack_size = STACK_SIZE;
    }
    new_thread->t_context = (ucontext_t *)malloc(sizeof(ucontext_t));
    new_thread->t_context->uc_link = 0;
    new_thread->t_context->uc_stack.ss_sp = allocate_stack(new_thread->stack_size);
    new_thread->t_context->uc_stack.ss_size = new_thread->stack_size;
    new_thread->t_context->uc_flags = 0;

    getcontext(new_thread->t_context);
    makecontext(new_thread->t_context, start_routine, 0);

    if (new_thread->thread_id == EINVAL || new_thread->thread_id == ENOMEM)
    {
        printf("Stack problem");
    }

    new_thread->state = READY;
    enqueue(new_thread, ready_queue);

    *t = new_thread->thread_id;

    strt_timer(&timer);
    return 0;
}

// Function to wait for a specific thread to terminate
int join_thread(thread_t thread, void **retval)
{
    strt_timer(&timer);
    tcb *this_thread, *waitfor_thread;
    // find the node having that particular thread id
    waitfor_thread = getthread_q(ready_queue, thread);
    if (thread == curr_thread->thread_id)
    {
        return EDEADLK;
    }

    /* If the thread is already EXITED, no need to wait. Just collect the return
     * value and exit
     */
    if (waitfor_thread->state == EXITED)
    {
        if (retval)
            *retval = waitfor_thread->ret_val;
        return 0;
    }

    /* If the thread is not EXITED and someone else is already waiting on it
     * return an error
     */
    if (waitfor_thread->blocked_join != NULL)
        return -1;
    waitfor_thread->blocked_join = curr_thread;
    curr_thread->state = BLOCKED;
    // othewise loop until the process is terminated
    while (waitfor_thread->state != EXITED)
        ;
    /* Target thread died, collect return value and return */
    if (retval)
        *retval = waitfor_thread->ret_val;
    stp_timer(&timer);
    return 0;
}

// Function to make a thread terminate itself , return value of the thread to be available to thread_join()
void exit_thread(void *retval)
{
    strt_timer(&timer);
    if (curr_thread->state == EXITED)
    {
        return;
    }

    curr_thread->ret_val = retval;
    // check if the thread is already blocked or not
    if (curr_thread->blocked_join != NULL)
    {
        curr_thread->blocked_join->state = READY;
    }

    curr_thread->state = EXITED;
    setcontext(ucp);
    stp_timer(&timer);
    return;
}