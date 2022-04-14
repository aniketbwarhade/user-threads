#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <signal.h>
#include "doublyll.h"

void *func_totestjoin(void *args)
{
    int *temp = (int *)args;

    printf("\nFunction Value: %d\n", *temp);
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

int main()
{
    init_thread_l();
    add_main_thread();
    thread_t t1, t2, t3, pid;
    int *status1 = NULL, *status2 = NULL, *status3 = NULL;
    int i = 11;
    int *temp = &i;
    // create_new_thread(&t1, NULL, func_totestjoin, (void *)temp);
    // create_new_thread(&t2, NULL, func_totestexit, NULL);
    create_new_thread(&t3, NULL, func_totestkill, NULL);
    create_new_thread(&t2, NULL, func_totestexit, NULL);
    exit_thread(temp);
    printf("hii after exit_thread in main");
    // join_thread(t3, (void **)&status1);
    // join_thread(t2, (void **)&status2);
    // join_thread(t3, (void **)&status3);
    // kill_thread(t3, SIGKILL);
    return 0;
}

/*
    {code to test thread_join}
    int *status1 = NULL, *status2 = NULL;
    int i = 11;
    int *temp = &i;
    create_new_thread(&tid, NULL, &func_totestjoin, (void *)temp);
    join_thread(tid, (void **)&status1);
    printf("%d\n", *status1);
*/

/*
    {code to test thread_exit}
    create_new_thread(&tid, NULL, &func_totestexit, NULL);
    create_new_thread(&pid, NULL, &func_totestexit, NULL);
    join_thread(tid, (void **)&status1);
    join_thread(pid, (void **)&status2);
    printf("%d\n", *status1);
    printf("%d\n", *status2);

*/