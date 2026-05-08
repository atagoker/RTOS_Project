#include <stdio.h>       // standard input/output
#include <pthread.h>     // posix threads library

#define NUM_THREADS 16   // number of threads to create
#define ITERATIONS  1000 // how many times each thread adds the value

long global_value = 0;   // shared variable all threads will modify
int user_value = 0;      // value entered by the user

void *add_value(void *arg) {              // thread function
    for (int i = 0; i < ITERATIONS; i++) { // loop 1000 times
        global_value += user_value;         // add user value to shared variable (no protection)
    }
    return NULL;                            // thread returns nothing
}

int main() {
    pthread_t threads[NUM_THREADS];        // array to store thread ids

    printf("Enter value to add: ");        // prompt user for input
    scanf("%d", &user_value);              // read user input

    for (int i = 0; i < NUM_THREADS; i++) {              // loop to create threads
        pthread_create(&threads[i], NULL, add_value, NULL); // create each thread
    }

    for (int i = 0; i < NUM_THREADS; i++) { // loop to wait for threads
        pthread_join(threads[i], NULL);      // wait for each thread to finish
    }

    printf("Current value is %ld\n", global_value); // print final result
    return 0;                                        // exit program
}
