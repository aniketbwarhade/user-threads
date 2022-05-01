#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <signal.h>
#include "queue.h"

int counter = 1;

void *func_totestjoin(void *args)
{
    int *temp = (int *)args;

    // printf("\nFunction Value: %d\n", *temp);
    return (void *)temp;
}

void *func_totestexit(void *args)
{
    int i = 200;
    printf("%lu Calling thread_exit\n", get_self_thread_id());
    // exit_thread(&i);
    printf("noprint");
    return NULL;
}

void *func_totestkill(void *args)
{
    int a = 3, b = 5;
    printf("sum is %d", a + b);
    printf("add");
    return NULL;
}

void *simple_func(void *args)
{
    return NULL;
}

void *killfun(void *arg)
{
    while (counter)
        ;
    return (void *)128;
}

void handler1(int sig)
{
    counter = 0;
    return;
}

void handler2(int sig)
{
    counter++;
    return;
}

int main()
{
    init_thread_q();
    add_main_thread();
    thread_t t1, t2, t3, t4, t5, pid;
    int *status1 = NULL, *status2 = NULL, *status3 = NULL;
    int i = 11;
    int *temp = &i;

    printf("ONE-ONE THREADING MODEL TESTING \n");

    int check1 = create_new_thread(&t1, NULL, simple_func, (void *)temp);
    // Value Return 0 by successfull creation of new thread, else return of error value.
    printf("-----------------Test case for creation of successful thread-----------------\n");
    if (check1 == 0)
    {
        printf("Test Passed\n");
    }
    else
    {
        printf("Test Failed\n");
    }
    printf("-----------------Test case for thread creation with null function -----------------\n");
    int check2 = create_new_thread(&t2, NULL, NULL, (void *)temp);
    // Value return 22 as this a invalid argument for the creation of new thread which represents the unsucessfull creation of new thread.
    if (check2 == 22)
    {
        printf("Test Passed\n");
    }
    else
    {
        printf("Test Failed\n");
    }
    printf("----------------- Test case for creation  thread with invalid argument -----------------\n");
    int check3 = create_new_thread(NULL, NULL, simple_func, (void *)temp);
    // Value return 22 as this a invalid argument for the creation of new thread which represents the unsucessfull creation of new thread.
    if (check3 == 22)
    {
        printf("Test Passed\n");
    }
    else
    {
        printf("Test Failed\n");
    }
    printf("-----------------Test case for Join_thread return value -----------------\n");

    create_new_thread(&t4, NULL, func_totestjoin, (void *)temp);
    join_thread(t4, (void **)&status2);
    if (*status2 == *temp)
    {
        printf("Test Passed\n");
    }
    else
    {
        printf("Test Failed\n");
    }

    printf("-----------------Test case for kill_thread -----------------\n");
    signal(SIGUSR1, handler1);
    counter = 1;
    create_new_thread(&t5, NULL, killfun, NULL);
    kill_thread(t5, SIGUSR1);
    join_thread(t5, (void **)&status3);
    if (counter == 0)
    {
        printf("Test Passed\n");
    }
    else
    {
        printf("Test Failed\n");
    }

    return 0;
}
