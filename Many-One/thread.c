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
#include "doublyll.h"

int tid_count = 0;
list *ready_queue;
static struct itimerval timer;
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
    thread->ret_val = thread->thread_start(thread->args);
    thread->state = EXITED;
    setcontext(ucp);
}

/*
 *   Whenever sigalrm is raised scheduler is called
 */
void scheduler()
{
    stp_timer(&timer);
    int count = threads_count(ready_queue);
    void *old;
    old = get_cthread();
    while (count > 0)
    {
        curr_thread = dequeue_thread(ready_queue);

        if (curr_thread->state == READY)
        {
            // as glibc doesn't support arch_prctl so we have to the corresponding syscall
            syscall(SYS_arch_prctl, ARCH_SET_FS, curr_thread); // access thread specific memory (TLS) in fs register
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

            syscall(SYS_arch_prctl, ARCH_SET_FS, old);
            switch (curr_thread->state)
            {
            case EXITED:
                enqueue(curr_thread, ready_queue);
                break;

            default:
                break;
            }
        }
        else if (curr_thread->state = EXITED)
        {
            strt_timer(&timer);
            enqueue(curr_thread, ready_queue);
            stp_timer(&timer);
        }
        else
        {
            return;
        }
        count--;
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
int mythread_init(void)
{
    curr_thread = (tcb *)malloc(sizeof(tcb));
    curr_thread->args = NULL;
    curr_thread->ret_val = NULL;
    curr_thread->thread_start = NULL;
    curr_thread->stack = NULL;
    curr_thread->stack_size = 0;
    curr_thread->thread_id = tid_count++;
    curr_thread->t_context = (ucontext_t *)malloc(sizeof(ucontext_t));
    getcontext(curr_thread->t_context);
    ready_queue = (list *)malloc(sizeof(list));
    ready_queue->head = ready_queue->tail = NULL;

    // setting signal for timer
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
int create_new_thread(thread_t *thread, thread_attr_t *attr, void *(*thread_start)(void *), void *args)
{

    if (!thread || !thread_start)
    {
        errno = EINVAL;
        return errno;
    }

    tcb *new_thread = (tcb *)malloc(sizeof(tcb));
    if (!new_thread)
    {
        errno = ENOMEM;
        return errno;
    }

    stp_timer(&timer);

    new_thread->thread_id = tid_count++;
    new_thread->state = RUNNING;
    new_thread->thread_start = thread_start;
    new_thread->args = args;
    if (attr)
    {
        new_thread->stack_size = attr->stack_size;
    }
    else
    {
        new_thread->stack_size = STACK_SIZE;
    }
    new_thread->stack = allocate_stack(new_thread->stack_size);
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

    *thread = new_thread->thread_id;

    strt_timer(&timer);
    return 0;
}
