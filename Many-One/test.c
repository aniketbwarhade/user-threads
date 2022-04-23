#define _GNU_SOURCE
#include <sys/wait.h>
#include <sys/utsname.h>
#include <sched.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <malloc.h>
#include <signal.h>
#include <errno.h>
#include <sys/syscall.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <ucontext.h>
#include "doublyll.h"

void *fn1(void *arg)
{
    int i = 100;

    printf("into thread1\n");

    return NULL;
}

void *fn2(void *arg)
{
    int i = 200;

    printf("into thread2\n");
    return NULL;
}

int main()
{
    thread_t tid, pid;
    init_thread_l();
    create_new_thread(&tid, NULL, &fn1, NULL);
    create_new_thread(&pid, NULL, &fn2, NULL);
    printf("%lu %lu %lu\n", get_self_thread_id(), tid, pid);

    return 0;
}