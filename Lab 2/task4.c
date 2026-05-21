#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

#define NUM_THREADS 12
#define SAMPLES_PER_THREAD 1000000

double pi_estimates[NUM_THREADS];
int results_ready = 0;

pthread_mutex_t mutex;
pthread_cond_t cond;

// Each thread estimates PI independently using Monte Carlo sampling
void *estimate_pi(void *arg)
{
    int id = *(int *)arg;
    free(arg);

    unsigned int seed = id * 12345;
    long inside = 0;

    // Generate random points and count points inside the quarter circle
    for (long i = 0; i < SAMPLES_PER_THREAD; i++)
    {
        double x = (double)rand_r(&seed) / RAND_MAX;
        double y = (double)rand_r(&seed) / RAND_MAX;

        if (x * x + y * y <= 1.0)
        {
            inside++;
        }
    }

    // Store this thread's PI estimation result
    pi_estimates[id] = 4.0 * (double)inside / SAMPLES_PER_THREAD;

    printf("Thread %d estimated pi = %.10f\n", id, pi_estimates[id]);

    // Notify the main thread that one more result is ready
    pthread_mutex_lock(&mutex);

    results_ready++;
    pthread_cond_signal(&cond);

    pthread_mutex_unlock(&mutex);

    return NULL;
}

int main()
{
    pthread_t threads[NUM_THREADS];

    // Initialize synchronization tools
    pthread_mutex_init(&mutex, NULL);
    pthread_cond_init(&cond, NULL);

    // Create worker threads for parallel PI estimation
    for (int i = 0; i < NUM_THREADS; i++)
    {
        int *id = malloc(sizeof(int));
        *id = i;

        pthread_create(&threads[i], NULL, estimate_pi, id);
    }

    // Main thread waits until all worker threads report their results
    pthread_mutex_lock(&mutex);

    while (results_ready < NUM_THREADS)
    {
        pthread_cond_wait(&cond, &mutex);
    }

    pthread_mutex_unlock(&mutex);

    // Join all threads after their work is completed
    for (int i = 0; i < NUM_THREADS; i++)
    {
        pthread_join(threads[i], NULL);
    }

    double mean = 0.0;

    // Calculate the final mean PI value from all thread estimates
    for (int i = 0; i < NUM_THREADS; i++)
    {
        mean += pi_estimates[i];
    }

    mean /= NUM_THREADS;

    printf("Mean value of pi = %.10f\n", mean);

    pthread_mutex_destroy(&mutex);
    pthread_cond_destroy(&cond);

    return 0;
}
