#include <stdio.h>
#include <unistd.h>

#include <alchemy/task.h>
#include <alchemy/timer.h>

RT_TASK periodic_task;

void task_function(void *arg)
{
    rt_task_set_periodic(NULL, TM_NOW, 1000000000);

    while (1)
    {
        rt_task_wait_period(NULL);

        printf("Periodic real-time task is running every 1 second\n");
    }
}

int main()
{
    rt_task_create(&periodic_task,
                   "PeriodicTask",
                   0,
                   50,
                   0);

    rt_task_start(&periodic_task,
                  &task_function,
                  NULL);

    pause();

    return 0;
}
