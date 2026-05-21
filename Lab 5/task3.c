#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>

#include <alchemy/task.h>
#include <alchemy/queue.h>

#define NUM_POINTS 100000

RT_TASK producer_task;
RT_TASK consumer_task;

RT_QUEUE queue;

typedef struct
{
    int inside;
    int total;
} ResultMessage;

void producer(void *arg)
{
    int i;
    int inside = 0;

    srand(time(NULL));

    for (i = 0; i < NUM_POINTS; i++)
    {
        double x = (double)rand() / RAND_MAX;
        double y = (double)rand() / RAND_MAX;

        if ((x * x + y * y) <= 1.0)
        {
            inside++;
        }
    }

    ResultMessage *msg;

    msg = rt_queue_alloc(&queue, sizeof(ResultMessage));

    msg->inside = inside;
    msg->total = NUM_POINTS;

    rt_queue_send(&queue,
                  msg,
                  sizeof(ResultMessage),
                  Q_NORMAL);

    printf("Producer sent data to queue\n");
}

void consumer(void *arg)
{
    ResultMessage *msg;
    ssize_t size;

    size = rt_queue_receive(&queue,
                            (void **)&msg,
                            TM_INFINITE);

    if (size > 0)
    {
        double pi;

        pi = 4.0 * ((double)msg->inside / msg->total);

        printf("Points inside circle: %d\n", msg->inside);
        printf("Total points: %d\n", msg->total);
        printf("Estimated PI value: %f\n", pi);

        rt_queue_free(&queue, msg);
    }
}

int main()
{
    rt_queue_create(&queue,
                    "MessageQueue",
                    1024,
                    10,
                    Q_FIFO);

    rt_task_create(&producer_task,
                   "ProducerTask",
                   0,
                   50,
                   0);

    rt_task_create(&consumer_task,
                   "ConsumerTask",
                   0,
                   50,
                   0);

    rt_task_start(&producer_task,
                  &producer,
                  NULL);

    rt_task_start(&consumer_task,
                  &consumer,
                  NULL);

    sleep(2);

    rt_queue_delete(&queue);

    return 0;
}
