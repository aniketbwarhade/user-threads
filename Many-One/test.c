#include <stdio.h>
#include <unistd.h>
#include <signal.h>
#include <string.h>
#include <stdint.h>
#include <inttypes.h>
#include <errno.h>
#include "thread.h"

int infinite = 1;

void check_test(int retval, int target)
{
    if (retval == target)
    {
        printf("Test Passed\n");
    }
    else
    {
        printf("Test Failed\n");
    }
}

void *func_totestjoin(void *args)
{
    int *temp = (int *)args;

    // printf("\nFunction Value: %d\n", *temp);
    return (void *)temp;
}

void *func_totestexit(void *args)
{
    int i = 200;
    exit_thread(&i);
    printf("hii");
}

void *fun(void *args)
{
    int a = 3;
    int *temp = &a;
    return (void *)temp;
}

void *simple_func(void *args)
{
    return NULL;
}

void *killfun(void *arg)
{
    while (infinite)
        ;
    return (void *)128;
}

void handler1(int sig)
{
    infinite = 0;
    return;
}

int main()
{
    add_main_thread();
    thread_t t1, t2, t3, t4, t5, t6, t7, pid;
    int *status1 = NULL, *status2 = NULL, *status3 = NULL, *status4 = NULL, *status5 = NULL;
    int i = 11;
    int *temp = &i;

    printf("MANY-ONE THREADING MODEL TESTING \n");

    int check1 = create_new_thread(&t1, NULL, simple_func, (void *)temp);
    // Value Return 0 by successfull creation of new thread, else return of error value.
    printf("-----------------Test case for creation of successful thread-----------------\n");
    check_test(check1, 0);

    printf("-----------------Test case for thread creation with null function -----------------\n");
    /* Testing with start routine NULL */
    int check2 = create_new_thread(&t2, NULL, NULL, (void *)temp);
    // Value return 22 as this a invalid argument for the creation of new thread which represents the unsucessfull creation of new thread.
    check_test(check2, 22);

    printf("----------------- Test case for creation  thread with invalid argument -----------------\n");
    /* Testing with invalid arguments */
    int check3 = create_new_thread(NULL, NULL, simple_func, (void *)temp);
    // Value return 22 as this a invalid argument for the creation of new thread which represents the unsucessfull creation of new thread.
    check_test(check3, 22);
    printf("-----------------Test case for Join_thread return value -----------------\n");

    create_new_thread(&t4, NULL, func_totestjoin, (void *)temp);
    join_thread(t4, (void **)&status2);
    check_test(*status2, *temp);

    printf("-----------------Test case for exit created thread Uses Return To Exit -----------------\n");
    /* Testing Created Thread Uses Return To Exit */
    create_new_thread(&t6, NULL, fun, NULL);
    join_thread(t6, (void **)&status4);
    check_test(*status4, 3);

    printf("-----------------Test case for exit created thread Uses thread_exit To Exit -----------------\n");
    /* Testing Created Thread Uses Return To Exit */
    create_new_thread(&t7, NULL, func_totestexit, NULL);
    join_thread(t7, (void **)&status5);
    check_test(*status5, 200);

    printf("-----------------Test case for kill_thread -----------------\n");
    signal(SIGUSR1, handler1);
    infinite = 1;
    create_new_thread(&t5, NULL, killfun, NULL);
    kill_thread(t5, SIGUSR1);
    join_thread(t5, (void **)&status3);
    check_test(infinite, 0);

    return 0;
}