#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include "list.h"

void *func(void *arg)
{
    int *temp = (int *)arg;
    printf("\nFunction Value: %d\n", *temp);
    return (void *)temp;
}

int main()
{
    init_thread_l();
    add_main_thread();
    thread_t tid, pid;
    int *status1 = NULL;
    int i = 11;
    int *temp = &i;
    create_new_thread(&tid, NULL, &func, (void *)temp);
    join_thread(tid, (void **)&status1);
    printf("%d\n", *status1);
    return 0;
}