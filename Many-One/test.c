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
    add_main_thread();
    thread_t t1, t2;
    printf("thread in many-one model\n");
    // int i = 42;
    // int *temp1 = &i;
    // int j = 47;
    // int *temp2 = &j;
    // int x = create_new_thread(&t1, NULL, fun, (void *)temp1);
    // int y = create_new_thread(&t2, NULL, fun, (void *)temp2);
    int *status1 = NULL;
    int i = 11;
    int *temp = &i;
    create_new_thread(&t1, NULL, &fun, (void *)temp);
    join_thread(t1, (void **)&status1);
    printf("status : %d\n", *status1);

    printf("succesfull");

    return 0;
}