#include <stdio.h>
#include <pthread.h>

#define NUM_THREADS 16
#define ITERATIONS 1000

long global_value = 0;
int user_value = 0;

// Thread function: each thread adds the user value many times
void *add_value(void *arg)
{
    for (int i = 0; i < ITERATIONS; i++)
    {
        // Shared variable is modified without protection
        global_value += user_value;
    }

    return NULL;
}

int main()
{
    pthread_t threads[NUM_THREADS];

    printf("Enter value to add: ");
    scanf("%d", &user_value);

    // Create 16 threads that run the same function
    for (int i = 0; i < NUM_THREADS; i++)
    {
        pthread_create(&threads[i], NULL, add_value, NULL);
    }

    // Wait until all threads finish execution
    for (int i = 0; i < NUM_THREADS; i++)
    {
        pthread_join(threads[i], NULL);
    }

    printf("Current value is %ld\n", global_value);

    return 0;
}
