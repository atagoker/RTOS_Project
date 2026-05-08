#include <stdio.h>    // standard input/output
#include <pthread.h>  // posix threads library

#define NUM_THREADS 16   // number of threads to create
#define ITERATIONS  1000 // how many times each thread adds the value

long global_value = 0;      // shared variable protected by mutex
int user_value = 0;         // value entered by the user
pthread_mutex_t mutex;      // mutex to protect shared variable

void *add_value(void *arg) {                // thread function
    for (int i = 0; i < ITERATIONS; i++) { // loop 1000 times
        pthread_mutex_lock(&mutex);         // lock mutex before accessing shared variable
        global_value += user_value;         // safely add value to shared variable
        pthread_mutex_unlock(&mutex);       // unlock mutex after update
    }
    return NULL;                            // thread returns nothing
}

int main() {
    pthread_t threads[NUM_THREADS];         // array to store thread ids

    pthread_mutex_init(&mutex, NULL);       // initialize the mutex

    printf("Enter value to add: ");         // prompt user for input
    scanf("%d", &user_value);               // read user input

    for (int i = 0; i < NUM_THREADS; i++) {               // loop to create threads
        pthread_create(&threads[i], NULL, add_value, NULL); // create each thread
    }

    for (int i = 0; i < NUM_THREADS; i++) { // loop to wait for threads
        pthread_join(threads[i], NULL);      // wait for each thread to finish
    }

    printf("Current value is %ld\n", global_value); // print final result
    pthread_mutex_destroy(&mutex);                   // clean up mutex
    return 0;                                        // exit program
}
