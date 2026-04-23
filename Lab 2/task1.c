#include <stdio.h>
#include <pthread.h>

#define NUM_THREADS 16
#define ITERATIONS  1000

/* Shared global variable - no synchronization (race condition demo) */
long global_value = 0;
int user_value = 0;

void *add_value(void *arg) {
    for (int i = 0; i < ITERATIONS; i++) {
        global_value += user_value;  /* Unsynchronized access - causes race condition */
    }
    return NULL;
}

int main() {
    pthread_t threads[NUM_THREADS];

    printf("Enter value to add: ");
    scanf("%d", &user_value);

    /* Create 16 threads */
    for (int i = 0; i < NUM_THREADS; i++) {
        pthread_create(&threads[i], NULL, add_value, NULL);
    }

    /* Wait for all threads to finish */
    for (int i = 0; i < NUM_THREADS; i++) {
        pthread_join(threads[i], NULL);
    }

    printf("Current value is %ld\n", global_value);
    return 0;
}
