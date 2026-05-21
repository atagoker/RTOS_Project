#include <stdio.h>
#include <pthread.h>

#define NUM_THREADS 16
#define ITERATIONS 1000

long global_value = 0;
int user_value = 0;

pthread_mutex_t mutex;

// Thread function: each thread safely adds the user value
void *add_value(void *arg)
{
    for (int i = 0; i < ITERATIONS; i++)
    {
        // Critical section protected by mutex
        pthread_mutex_lock(&mutex);

        global_value += user_value;

        pthread_mutex_unlock(&mutex);
    }

    return NULL;
}

int main()
{
    pthread_t threads[NUM_THREADS];

    // Initialize mutex before using it
    pthread_mutex_init(&mutex, NULL);

    printf("Enter value to add: ");
    scanf("%d", &user_value);

    // Create 16 threads that update the same shared variable
    for (int i = 0; i < NUM_THREADS; i++)
    {
        pthread_create(&threads[i], NULL, add_value, NULL);
    }

    // Wait for all threads to finish
    for (int i = 0; i < NUM_THREADS; i++)
    {
        pthread_join(threads[i], NULL);
    }

    printf("Current value is %ld\n", global_value);

    // Destroy mutex after it is no longer needed
    pthread_mutex_destroy(&mutex);

    return 0;
}
