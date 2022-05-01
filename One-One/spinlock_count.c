#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include "./locks.h"
#include "queue.h"
spinlock lock;

int c = 0;
int c1 = 0;
int c2 = 0;
int r = 1;

void *fun1(void *arg)
{
    while (r)
    {

        thread_spin_lock(&lock);
        c++;
        thread_spin_unlock(&lock);
        c1++;
    }
    return NULL;
}

void *fun2(void *arg)
{
    while (r)
    {
        thread_spin_lock(&lock);

        c++;
        thread_spin_unlock(&lock);
        c2++;
    }
    return NULL;
}

int main()
{
    init_thread_q();
    add_main_thread();
    thread_t t1, t2;
    thread_spin_init(&lock);
    create_new_thread(&t1, NULL, &fun1, NULL);
    create_new_thread(&t2, NULL, &fun2, NULL);
    sleep(1);
    r = 0;
    join_thread(t1, NULL);
    join_thread(t2, NULL);
    printf("\n---------------------------------------------------------------------------------------\n");
    printf("Testcases to check synchronization using spinlock ( Race Condition ):-\n");
    printf(" c1 = %d\n", c1);
    printf(" c2 = %d\n", c2);
    printf(" c1 + c2 = %d\n", c1 + c2);
    printf(" c = %d\n", c);
    if (c == (c1 + c2))
    {
        printf("\nTest Passed\n");
    }
    else
    {
        printf("\nTest Failed\n");
    }
    return 0;
}
