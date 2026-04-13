#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <string.h>

#define SHM_SIZE 1024

typedef struct {
    int ready;              // 0 = empty, 1 = data ready, 2 = finished
    char buffer[SHM_SIZE];
} shared_data;

int main() {
    int fd;
    shared_data *shm;

    // Create shared memory
    fd = shm_open("/my_shm", O_CREAT | O_RDWR, 0666);
    ftruncate(fd, sizeof(shared_data));
    shm = mmap(NULL, sizeof(shared_data), PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);

    shm->ready = 0;

    pid_t pid = fork();

    if (pid == 0) {
        // READER PROCESS
        FILE *out = fopen("output.txt", "w");

        while (1) {
            while (shm->ready == 0); // wait for data

            if (shm->ready == 2) break;

            fprintf(out, "%s", shm->buffer);

            shm->ready = 0; // mark buffer free
        }

        fclose(out);
    } else {
        // WRITER PROCESS
        FILE *in = fopen("input.txt", "r");
        char temp[SHM_SIZE];

        while (fgets(temp, SHM_SIZE, in)) {
            while (shm->ready == 1); // wait until reader consumes

            strncpy(shm->buffer, temp, SHM_SIZE - 1);
            shm->buffer[SHM_SIZE - 1] = '\0';

            shm->ready = 1; // signal data ready
        }

        shm->ready = 2; // signal end of file

        fclose(in);
        wait(NULL);

        // Cleanup
        munmap(shm, sizeof(shared_data));
        shm_unlink("/my_shm");
    }

    return 0;
}
