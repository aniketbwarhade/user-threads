#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include "list.h"

void *func(void *arg)
{
    printf("Child pid : %d     { from getpid() }\n", getpid());
    printf("\n I am Child process.\n");
    printf("%d", 3 + 5);
    return NULL;
}

int main()
{
    init_thread_l();
    add_main_thread();
    thread_t tid, pid;
    int *status1 = NULL, *status2 = NULL;

    printf("in test1\n");

    create_new_thread(&tid, NULL, &func, NULL);
    // create_new_thread(&pid, NULL, &func, NULL);

    join_thread(tid, (void **)&status1);
    // join_thread(pid, (void **)&status2);

    printf("%d %d\n", *status1, *status2);
    return 0;
}