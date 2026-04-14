#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <mqueue.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <time.h>

#define QUEUE_NAME "/pi_queue"

typedef struct {
    int child_id;
    double pi_value;
} message_t;

int main() {
    int num_children, num_points;
    pid_t pid;
    mqd_t mq;
    struct mq_attr attr;
    message_t msg;
    double sum_pi = 0.0;

    printf("Enter number of child processes: ");
    scanf("%d", &num_children);

    printf("Enter number of points per child: ");
    scanf("%d", &num_points);

    attr.mq_flags = 0;
    attr.mq_maxmsg = 10;
    attr.mq_msgsize = sizeof(message_t);
    attr.mq_curmsgs = 0;

    mq_unlink(QUEUE_NAME);
    mq = mq_open(QUEUE_NAME, O_CREAT | O_RDONLY, 0666, &attr);
    if (mq == (mqd_t)-1) {
        perror("mq_open failed");
        return 1;
    }

    for (int i = 0; i < num_children; i++) {
        pid = fork();

        if (pid < 0) {
            perror("fork failed");
            return 1;
        }

        if (pid == 0) {
            mqd_t child_mq;
            int inside_circle = 0;
            double x, y, pi_estimate;

            child_mq = mq_open(QUEUE_NAME, O_WRONLY);
            if (child_mq == (mqd_t)-1) {
                perror("child mq_open failed");
                exit(1);
            }

            srand(time(NULL) ^ getpid());

            for (int j = 0; j < num_points; j++) {
                x = (double)rand() / RAND_MAX;
                y = (double)rand() / RAND_MAX;

                if ((x * x + y * y) <= 1.0) {
                    inside_circle++;
                }
            }

            pi_estimate = 4.0 * inside_circle / num_points;

            msg.child_id = i + 1;
            msg.pi_value = pi_estimate;

            if (mq_send(child_mq, (const char *)&msg, sizeof(message_t), 0) == -1) {
                perror("mq_send failed");
                mq_close(child_mq);
                exit(1);
            }

            mq_close(child_mq);
            exit(0);
        }
    }

    for (int i = 0; i < num_children; i++) {
        if (mq_receive(mq, (char *)&msg, sizeof(message_t), NULL) == -1) {
            perror("mq_receive failed");
            mq_close(mq);
            mq_unlink(QUEUE_NAME);
            return 1;
        }

        printf("Child %d estimated pi = %f\n", msg.child_id, msg.pi_value);
        sum_pi += msg.pi_value;
    }

    for (int i = 0; i < num_children; i++) {
        wait(NULL);
    }

    printf("Final mean value of pi = %f\n", sum_pi / num_children);

    mq_close(mq);
    mq_unlink(QUEUE_NAME);

    return 0;
}
