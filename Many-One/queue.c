#include "queue.h"
void addthread_to_ready(thread_t tid)
{
    sched_thread *tmp;
    tmp = (sched_thread *)malloc(sizeof(sched_thread));
    tmp->tid = tid;
    if (sched_queue == NULL)
    {
        sched_queue = tmp;
        tmp->next = tmp->prev = tmp;
        return;
    }
    sched_queue->prev->next = tmp;
    tmp->prev = sched_queue->prev;
    tmp->next = sched_queue;
    sched_queue->prev = tmp;
    return;
}

tcb *findnext_ready(void)
{
    thread_t t = curr_running->tid;
    sched_thread *tmp = sched_queue;
    if (!tmp)
    {
        return NULL;
    }

    do
    {
        if (tmp->tid == t)
        {
            return getthread_l(tmp->next->tid);
        }
        tmp = tmp->next;
    } while (tmp != sched_queue);

    return search_thread(sched_queue->tid);
}

void removefrom_ready(tcb *thread)
{
    sched_thread *tmp = sched_queue;
    if (!tmp)
        return;

    do
    {
        if (tmp->tid == thread->thread_id)
        {
            if (sched_queue == tmp)
            {
                if (sched_queue->next == sched_queue)
                {
                    tmp->next = tmp->prev = NULL;
                    free(tmp);
                    sched_queue = NULL;
                }
                else
                {
                    sched_queue = sched_queue->next;
                    sched_queue->prev = tmp->prev;
                    if (sched_queue->next == tmp)
                        sched_queue->next = sched_queue;
                    tmp->next = tmp->prev = NULL;
                    free(tmp);
                }
                return;
            }
            else
            {
                tmp->prev->next = tmp->next;
                tmp->next->prev = tmp->prev;
                tmp->next = tmp->prev = NULL;
                free(tmp);
            }
            return;
        }
        tmp = tmp->next;
    } while (tmp != sched_queue);
}