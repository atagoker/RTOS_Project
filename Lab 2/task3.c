#include <stdio.h>      // standard input/output
#include <string.h>     // memset and string functions
#include <pthread.h>    // posix threads library
#include <semaphore.h>  // posix semaphores

#define BUFFER_SIZE 5   // maximum number of chars in the book at once
#define TEXT_LEN    20  // total number of chars to write

char book[BUFFER_SIZE];                        // shared buffer (the "book")
char text_to_write[TEXT_LEN + 1] = "ABCDEFGHIJKLMNOPQRST"; // text the writer will write

int write_pos  = 0;   // current write position in the buffer
int read_pos   = 0;   // current read position in the buffer
int text_index = 0;   // index tracking how far into the text we've written

sem_t empty_slots;   // counts how many free slots are in the buffer
sem_t filled_slots;  // counts how many filled slots are in the buffer
sem_t mutex;         // binary semaphore for mutual exclusion

void print_book() {                          // helper to print buffer state
    printf("Book: [");                       // opening bracket
    for (int i = 0; i < BUFFER_SIZE; i++) { // loop through each slot
        if (book[i] == '\0')                 // check if slot is empty
            printf("_");                     // print underscore for empty slot
        else
            printf("%c", book[i]);           // print character if slot is filled
        if (i < BUFFER_SIZE - 1) printf(" "); // space between slots
    }
    printf("]\n");                           // closing bracket
}

void *writer(void *arg) {                        // writer thread function
    while (text_index < TEXT_LEN) {              // loop until all chars are written
        sem_wait(&empty_slots);                  // wait for a free slot
        sem_wait(&mutex);                        // enter critical section

        char c = text_to_write[text_index++];   // get next char from text
        book[write_pos] = c;                     // write char into buffer
        printf("Writer wrote: %c at page %d\n", c, write_pos); // log the write
        print_book();                            // show buffer state
        write_pos = (write_pos + 1) % BUFFER_SIZE; // advance write position (circular)

        sem_post(&mutex);                        // exit critical section
        sem_post(&filled_slots);                 // signal that a slot is now filled
    }
    return NULL;                                 // writer thread done
}

void *reader(void *arg) {                        // reader thread function
    int chars_read = 0;                          // count of chars read so far
    while (chars_read < TEXT_LEN) {              // loop until all chars are read
        sem_wait(&filled_slots);                 // wait for a filled slot
        sem_wait(&mutex);                        // enter critical section

        char c = book[read_pos];                 // read char from buffer
        printf("Reader read: %c from page %d\n", c, read_pos); // log the read
        book[read_pos] = '\0';                   // clear the slot after reading
        print_book();                            // show buffer state
        read_pos = (read_pos + 1) % BUFFER_SIZE; // advance read position (circular)
        chars_read++;                            // increment read counter

        sem_post(&mutex);                        // exit critical section
        sem_post(&empty_slots);                  // signal that a slot is now free
    }
    return NULL;                                 // reader thread done
}

int main() {
    pthread_t writer_thread, reader_thread;   // thread handles for writer and reader

    memset(book, '\0', sizeof(book));         // initialize buffer with null chars

    sem_init(&empty_slots,  0, BUFFER_SIZE);  // all slots start as empty
    sem_init(&filled_slots, 0, 0);            // no slots filled at start
    sem_init(&mutex,        0, 1);            // binary semaphore starts unlocked

    pthread_create(&writer_thread, NULL, writer, NULL); // start writer thread
    pthread_create(&reader_thread, NULL, reader, NULL); // start reader thread

    pthread_join(writer_thread, NULL);        // wait for writer to finish
    pthread_join(reader_thread, NULL);        // wait for reader to finish

    sem_destroy(&empty_slots);                // clean up empty slots semaphore
    sem_destroy(&filled_slots);               // clean up filled slots semaphore
    sem_destroy(&mutex);                      // clean up mutex semaphore

    return 0;                                 // exit program
}
