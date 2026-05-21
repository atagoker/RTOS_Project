#include <stdio.h>
#include <string.h>
#include <pthread.h>
#include <semaphore.h>

#define BUFFER_SIZE 5
#define TEXT_LEN 20

char book[BUFFER_SIZE];
char text_to_write[TEXT_LEN + 1] = "ABCDEFGHIJKLMNOPQRST";

int write_pos = 0;
int read_pos = 0;
int text_index = 0;

sem_t empty_slots;
sem_t filled_slots;
sem_t mutex;

// Helper function used only to display the current buffer state
void print_book()
{
    printf("Book: [");

    for (int i = 0; i < BUFFER_SIZE; i++)
    {
        if (book[i] == '\0')
            printf("_");
        else
            printf("%c", book[i]);

        if (i < BUFFER_SIZE - 1)
            printf(" ");
    }

    printf("]\n");
}

void *writer(void *arg)
{
    while (text_index < TEXT_LEN)
    {
        // Writer waits until there is at least one empty page
        sem_wait(&empty_slots);

        // Only one thread can access the shared book at a time
        sem_wait(&mutex);

        char c = text_to_write[text_index++];

        book[write_pos] = c;

        printf("Writer wrote: %c at page %d\n", c, write_pos);
        print_book();

        // Circular buffer logic
        write_pos = (write_pos + 1) % BUFFER_SIZE;

        sem_post(&mutex);

        // Signal that one more page is filled
        sem_post(&filled_slots);
    }

    return NULL;
}

void *reader(void *arg)
{
    int chars_read = 0;

    while (chars_read < TEXT_LEN)
    {
        // Reader waits until there is at least one written page
        sem_wait(&filled_slots);

        // Only one thread can access the shared book at a time
        sem_wait(&mutex);

        char c = book[read_pos];

        printf("Reader read: %c from page %d\n", c, read_pos);

        book[read_pos] = '\0';

        print_book();

        // Circular buffer logic
        read_pos = (read_pos + 1) % BUFFER_SIZE;

        chars_read++;

        sem_post(&mutex);

        // Signal that one more page became empty
        sem_post(&empty_slots);
    }

    return NULL;
}

int main()
{
    pthread_t writer_thread, reader_thread;

    // Initialize the book as empty
    memset(book, '\0', sizeof(book));

    // Counting semaphores control empty and filled pages
    sem_init(&empty_slots, 0, BUFFER_SIZE);
    sem_init(&filled_slots, 0, 0);

    // Binary semaphore works like a mutex for the shared book
    sem_init(&mutex, 0, 1);

    pthread_create(&writer_thread, NULL, writer, NULL);
    pthread_create(&reader_thread, NULL, reader, NULL);

    pthread_join(writer_thread, NULL);
    pthread_join(reader_thread, NULL);

    sem_destroy(&empty_slots);
    sem_destroy(&filled_slots);
    sem_destroy(&mutex);

    return 0;
}
