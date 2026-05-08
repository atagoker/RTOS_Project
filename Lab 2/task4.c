#include <stdio.h>    // standard input/output
#include <stdlib.h>   // malloc and free
#include <pthread.h>  // posix threads library

#define NUM_THREADS        12      // number of threads for pi estimation
#define SAMPLES_PER_THREAD 1000000 // random samples each thread generates

double pi_estimates[NUM_THREADS];  // stores each thread's pi result
int results_ready = 0;             // counts how many threads have finished

pthread_mutex_t mutex;  // mutex to protect results_ready counter
pthread_cond_t  cond;   // condition variable to signal main thread

void *estimate_pi(void *arg) {
    int id = *(int *)arg;   // get thread id from argument
    free(arg);              // free the allocated id memory

    unsigned int seed = id * 12345; // unique seed per thread for random numbers
    long inside = 0;                // counts points that fall inside the circle

    for (long i = 0; i < SAMPLES_PER_THREAD; i++) { // loop through all samples
        double x = (double)rand_r(&seed) / RAND_MAX; // random x between 0 and 1
        double y = (double)rand_r(&seed) / RAND_MAX; // random y between 0 and 1
        if (x * x + y * y <= 1.0)                    // check if point is inside circle
            inside++;                                 // count the point
    }

    pi_estimates[id] = 4.0 * (double)inside / SAMPLES_PER_THREAD; // calculate pi estimate
    printf("Thread %d estimated pi = %.10f\n", id, pi_estimates[id]); // print result

    pthread_mutex_lock(&mutex);    // lock before updating shared counter
    results_ready++;               // increment finished thread counter
    pthread_cond_signal(&cond);    // wake up main thread to check progress
    pthread_mutex_unlock(&mutex);  // unlock after update

    return NULL;                   // thread done
}

int main() {
    pthread_t threads[NUM_THREADS]; // array to store thread ids

    pthread_mutex_init(&mutex, NULL); // initialize mutex
    pthread_cond_init(&cond, NULL);   // initialize condition variable

    for (int i = 0; i < NUM_THREADS; i++) { // loop to create all threads
        int *id = malloc(sizeof(int));       // allocate memory for thread id
        *id = i;                             // set the thread id value
        pthread_create(&threads[i], NULL, estimate_pi, id); // create thread
    }

    pthread_mutex_lock(&mutex);                  // lock before checking condition
    while (results_ready < NUM_THREADS) {        // loop until all threads are done
        pthread_cond_wait(&cond, &mutex);         // wait for a signal from a thread
    }
    pthread_mutex_unlock(&mutex);                // unlock after all results are in

    for (int i = 0; i < NUM_THREADS; i++) { // loop to join all threads
        pthread_join(threads[i], NULL);      // wait for each thread to finish
    }

    double mean = 0.0;                       // variable to accumulate pi values
    for (int i = 0; i < NUM_THREADS; i++) { // loop through all estimates
        mean += pi_estimates[i];             // sum all pi estimates
    }
    mean /= NUM_THREADS;                     // divide by thread count to get mean

    printf("Mean value of pi = %.10f\n", mean); // print final mean pi value

    pthread_mutex_destroy(&mutex); // clean up mutex
    pthread_cond_destroy(&cond);   // clean up condition variable

    return 0;                      // exit program
}
