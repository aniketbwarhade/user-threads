#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include "thread.h"

void *func(void *arg)
{
    printf("Child pid : %d     { from getpid() }\n", getpid());
    printf("\n I am Child process.\n");
    return NULL;
}

int main()
{
    thread_t tid;
    printf("%lu\n", tid);
    create_new_thread(&tid, NULL, func, NULL);
    printf("%lu\n", tid);

    return 0;
}