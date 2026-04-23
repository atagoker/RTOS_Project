#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

#define NUM_THREADS        12
#define SAMPLES_PER_THREAD 1000000

/* Shared results array and synchronization */
double pi_estimates[NUM_THREADS];
int results_ready = 0;

pthread_mutex_t mutex;
pthread_cond_t  cond;

void *estimate_pi(void *arg) {
    int id = *(int *)arg;
    free(arg);

    unsigned int seed = id * 12345;
    long inside = 0;

    /* Monte Carlo sampling - count points inside unit circle */
    for (long i = 0; i < SAMPLES_PER_THREAD; i++) {
        double x = (double)rand_r(&seed) / RAND_MAX;
        double y = (double)rand_r(&seed) / RAND_MAX;
        if (x * x + y * y <= 1.0)
            inside++;
    }

    /* Estimate pi using ratio of areas */
    pi_estimates[id] = 4.0 * (double)inside / SAMPLES_PER_THREAD;
    printf("Thread %d estimated pi = %.10f\n", id, pi_estimates[id]);

    /* Signal main thread that this result is ready */
    pthread_mutex_lock(&mutex);
    results_ready++;
    pthread_cond_signal(&cond);
    pthread_mutex_unlock(&mutex);

    return NULL;
}

int main() {
    pthread_t threads[NUM_THREADS];

    pthread_mutex_init(&mutex, NULL);
    pthread_cond_init(&cond, NULL);

    /* Create all estimation threads */
    for (int i = 0; i < NUM_THREADS; i++) {
        int *id = malloc(sizeof(int));
        *id = i;
        pthread_create(&threads[i], NULL, estimate_pi, id);
    }

    /* Wait until all threads have reported their result */
    pthread_mutex_lock(&mutex);
    while (results_ready < NUM_THREADS) {
        pthread_cond_wait(&cond, &mutex);
    }
    pthread_mutex_unlock(&mutex);

    for (int i = 0; i < NUM_THREADS; i++) {
        pthread_join(threads[i], NULL);
    }

    /* Compute mean pi value across all threads */
    double mean = 0.0;
    for (int i = 0; i < NUM_THREADS; i++) {
        mean += pi_estimates[i];
    }
    mean /= NUM_THREADS;

    printf("Mean value of pi = %.10f\n", mean);

    pthread_mutex_destroy(&mutex);
    pthread_cond_destroy(&cond);

    return 0;
}
