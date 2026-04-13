#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>

#define BUFFER_SIZE 1024

int main() {
    int pipe_fd[2];
    pid_t pid;
    char write_buffer[BUFFER_SIZE];
    char read_buffer[BUFFER_SIZE];

    // Create pipe
    if (pipe(pipe_fd) == -1) {
        perror("pipe failed");
        return 1;
    }

    // Create child process
    pid = fork();

    if (pid < 0) {
        perror("fork failed");
        return 1;
    }

    if (pid == 0) {
        // Child process: receiver
        close(pipe_fd[1]);

        ssize_t bytes_read = read(pipe_fd[0], read_buffer, BUFFER_SIZE - 1);
        if (bytes_read < 0) {
            perror("read failed");
            close(pipe_fd[0]);
            return 1;
        }

        read_buffer[bytes_read] = '\0';
        printf("Child process received: %s\n", read_buffer);

        close(pipe_fd[0]);
    } else {
        // Parent process: sender
        close(pipe_fd[0]);

        printf("Parent process - enter a message: ");
        if (fgets(write_buffer, BUFFER_SIZE, stdin) == NULL) {
            perror("input failed");
            close(pipe_fd[1]);
            return 1;
        }

        ssize_t bytes_written = write(pipe_fd[1], write_buffer, strlen(write_buffer));
        if (bytes_written < 0) {
            perror("write failed");
            close(pipe_fd[1]);
            return 1;
        }

        close(pipe_fd[1]);
        wait(NULL);
    }

    return 0;
}
