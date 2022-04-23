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
#include <setjmp.h>
#include <sys/time.h>
#include "doublyll.h"
// #include "thread.h"

struct sigaction action;
struct itimerval timer;
tcb *sched_thread;
int flag = 0;

// Return thread_t of calling thread
thread_t get_self_thread_id(void)
{
	thread_t self_tid;
	self_tid = gettid();
	return self_tid;
}
tcb *get_next_ready_thread(void)
{
	thread_t t = sched_thread->thread_id;
	node *tmp = thread_list->head;
	if (!tmp)
		return NULL;

	do
	{
		if (tmp->thread->thread_id = t)
		{
			return tmp->next->thread;
		}
		tmp = tmp->next;
	} while (tmp != NULL);

	return NULL;
}

void scheduler()
{
	tcb *n;

	if (setjmp(sched_thread->buf))
	{
		printf("again in main\n");
	}
	else
	{
		n = get_next_ready_thread();
		sched_thread = n;
		longjmp(n->buf, 1);
	}
}

int add_main_thread(void)
{
	tcb *m;
	m = (tcb *)malloc(sizeof(tcb));
	if (m == NULL)
	{
		perror("ERROR: Unable to allocate memory for main thread.\n");
		return errno;
	}

	m->start_func = NULL;
	m->args = NULL;
	m->state = THREAD_READY;
	m->return_val = NULL;
	m->blocked_join = NULL;
	m->thread_id = get_self_thread_id();
	addthread_l(m);
	return 0;
}
int create_new_thread(thread_t *t, const thread_attr_t *attr, void *(*start_function)(void *), void *arg)
{
	tcb *child_thread;
	int ret;

	if (!thread_list)
	{
		/* first create call
			Add main thread
		*/

		memset(&action, 0, sizeof(action));

		action.sa_handler = scheduler;

		sigaction(SIGVTALRM, &action, NULL);

		ret = add_main_thread();

		if (ret != 0)
		{
			return ret;
		}

		sched_thread = thread_list->head->thread;
	}

	/*
		Create thread_struct for child thread
	*/
	child_thread = (tcb *)malloc(sizeof(tcb));
	if (child_thread == NULL)
	{
		perror("ERROR: Unable to allocate memory for thread_struct.\n");
		return errno;
	}

	child_thread->start_func = start_function;
	child_thread->args = arg;
	child_thread->state = THREAD_READY;
	child_thread->return_val = NULL;
	child_thread->blocked_join = NULL;
	child_thread->thread_id = thread_list->tail->thread->thread_id + 1;
	*t = child_thread->thread_id;
	/* Add created thread_struct to list of thread blocks
	 */
	addthread_l(child_thread);
	if (setjmp(sched_thread->buf))
	{
	}
	else
	{
		sched_thread = child_thread;
		if (!flag)
		{

			/* Configure the timer to expire after 10 usec... */
			timer.it_value.tv_sec = 0;
			timer.it_value.tv_usec = 10;
			/* ... and every 10 usec after that. */
			timer.it_interval.tv_sec = 0;
			timer.it_interval.tv_usec = 10;

			setitimer(ITIMER_VIRTUAL, &timer, NULL);

			flag = 1;
		}

		start_function(arg);
	}

	return 0;
}