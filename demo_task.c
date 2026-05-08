#include <stdio.h>
#include <signal.h>
#include <unistd.h>
#include <alchemy/task.h>

RT_TASK demo_task;

void demo(void *arg) {
    RT_TASK *curtask;
    RT_TASK_INFO curtaskinfo;

    printf("Hello World!\n");

    curtask = rt_task_self();
    rt_task_inquire(curtask, &curtaskinfo);

    printf("Task name: %s\n", curtaskinfo.name);
}

int main(int argc, char *argv[]) {

    char str[10];

    printf("start task\n");

    rt_task_create(&demo_task, "DemoTask", 0, 50, 0);
    rt_task_start(&demo_task, &demo, NULL);

    sleep(1);

    return 0;
}