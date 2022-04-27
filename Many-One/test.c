#include <stdio.h>
#include <unistd.h>
#include <signal.h>
#include <string.h>
#include <stdint.h>
#include <inttypes.h>
#include <errno.h>
#include "thread.h"

void *fun(void *arg)
{
    int *temp = (int *)arg;
    printf("Function Value: %d\n", *temp);
    return (void *)temp;
}

int main()
{
    mythread_init();
    thread_t t1, t2;
    printf("------Creating a thread in many-one model------\n");
    int i = 42;
    int *temp1 = &i;
    int j = 47;
    int *temp2 = &j;
    int x = create_new_thread(&t1, NULL, fun, (void *)temp1);
    int y = create_new_thread(&t2, NULL, fun, (void *)temp2);

    sleep(1);

    if (x == 0)
    {
        printf("-----Test1 Passed-----\n");
    }
    else
    {
        printf("-----Test1 failed-----\n");
    }
    if (y == 0)
    {
        printf("-----Test2 Passed-----\n");
    }
    else
    {
        printf("-----Test2 failed-----\n");
    }

    return 0;
}