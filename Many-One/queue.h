#include "doublyll.h"
typedef struct sched_thread
{
    thread_t tid;
    struct sched_thread *prev, *next;

} sched_thread;

sched_thread *sched_queue;
sched_thread *curr_running;