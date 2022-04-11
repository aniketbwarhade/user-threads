#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include "doublyll.h"

void *func_totestjoin(void *arg)
{
    int *temp = (int *)arg;
    printf("\nFunction Value: %d\n", *temp);
    return (void *)temp;
}
void *func_totestexit(void *arg)
{
    int i = 200;
    printf("%lu Calling thread_exit\n", get_self_thread_id());
    exit_thread(&i);
    printf("noprint");
    return NULL;
}

int main()
{
    init_thread_l();
    add_main_thread();
    thread_t tid, pid;
    int *status1 = NULL, *status2 = NULL;

    //{code to test thread_join}
    /* int i = 11;
    int *temp = &i;
    create_new_thread(&tid, NULL, &func_totestjoin, (void *)temp);
    join_thread(tid, (void **)&status1);
    printf("%d\n", *status1); */

    create_new_thread(&tid, NULL, &func_totestexit, NULL);
    create_new_thread(&pid, NULL, &func_totestexit, NULL);

    join_thread(tid, (void **)&status1);
    join_thread(pid, (void **)&status2);

    printf("%d %d\n", *status1, *status2);
    return 0;
}
