#include <stdio.h>
#include <string.h>
#include <pthread.h>
#include <semaphore.h>

#define BUFFER_SIZE 5
#define TEXT_LEN    20

/* Shared book buffer */
char book[BUFFER_SIZE];
char text_to_write[TEXT_LEN + 1] = "ABCDEFGHIJKLMNOPQRST";

int write_pos = 0;
int read_pos  = 0;
int text_index = 0;

sem_t empty_slots;  /* Counts free slots in the buffer */
sem_t filled_slots; /* Counts written slots in the buffer */
sem_t mutex;        /* Mutual exclusion for buffer access */

/* Print current state of the book */
void print_book() {
    printf("Book: [");
    for (int i = 0; i < BUFFER_SIZE; i++) {
        if (book[i] == '\0')
            printf("_");
        else
            printf("%c", book[i]);
        if (i < BUFFER_SIZE - 1) printf(" ");
    }
    printf("]\n");
}

void *writer(void *arg) {
    while (text_index < TEXT_LEN) {
        sem_wait(&empty_slots);   /* Wait for a free slot */
        sem_wait(&mutex);         /* Enter critical section */

        char c = text_to_write[text_index++];
        book[write_pos] = c;
        printf("Writer wrote: %c at page %d\n", c, write_pos);
        print_book();
        write_pos = (write_pos + 1) % BUFFER_SIZE;

        sem_post(&mutex);         /* Exit critical section */
        sem_post(&filled_slots);  /* Signal that a slot is filled */
    }
    return NULL;
}

void *reader(void *arg) {
    int chars_read = 0;
    while (chars_read < TEXT_LEN) {
        sem_wait(&filled_slots);  /* Wait for a filled slot */
        sem_wait(&mutex);         /* Enter critical section */

        char c = book[read_pos];
        printf("Reader read: %c from page %d\n", c, read_pos);
        book[read_pos] = '\0';    /* Clear the page after reading */
        print_book();
        read_pos = (read_pos + 1) % BUFFER_SIZE;
        chars_read++;

        sem_post(&mutex);         /* Exit critical section */
        sem_post(&empty_slots);   /* Signal that a slot is free */
    }
    return NULL;
}

int main() {
    pthread_t writer_thread, reader_thread;

    memset(book, '\0', sizeof(book));

    sem_init(&empty_slots,  0, BUFFER_SIZE);
    sem_init(&filled_slots, 0, 0);
    sem_init(&mutex,        0, 1);

    pthread_create(&writer_thread, NULL, writer, NULL);
    pthread_create(&reader_thread, NULL, reader, NULL);

    pthread_join(writer_thread, NULL);
    pthread_join(reader_thread, NULL);

    sem_destroy(&empty_slots);
    sem_destroy(&filled_slots);
    sem_destroy(&mutex);

    return 0;
}
