#include <stdio.h>
#include <unistd.h>

#include <alchemy/task.h>
#include <alchemy/mutex.h>

RT_TASK task1;
RT_TASK task2;

RT_MUTEX mutex;

int counter = 0;

void increment_task(void *arg)
{
    int i;

    for (i = 0; i < 100000; i++)
    {
        rt_mutex_acquire(&mutex, TM_INFINITE);

        counter++;

        rt_mutex_release(&mutex);
    }
}

int main()
{
    rt_mutex_create(&mutex, "CounterMutex");

    rt_task_create(&task1,
                   "Task1",
                   0,
                   50,
                   0);

    rt_task_create(&task2,
                   "Task2",
                   0,
                   50,
                   0);

    rt_task_start(&task1,
                  &increment_task,
                  NULL);

    rt_task_start(&task2,
                  &increment_task,
                  NULL);

    sleep(1);

    printf("Final counter value: %d\n", counter);

    rt_mutex_delete(&mutex);

    return 0;
}
