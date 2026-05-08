# RTOS Project – Lab 3

This repository contains solutions for Lab 3 tasks focused on process creation and interprocess communication in Linux using POSIX system calls. All programs are written in C and executed in a Linux environment.

## Tasks Overview

### Task 1 – Pipe Communication
A parent process reads user input and sends it through a pipe to a child process, which prints the received data.

### Task 2 – Readers and Writers (Shared Memory)
A writer process reads from an input file and writes data to shared memory, while a reader process waits, reads the data, and writes it into an output file so both files match.

### Task 3 – Monte Carlo Pi Estimation (Message Queues)
A server process creates child processes using fork(). Each child computes an approximation of pi and sends the result through a POSIX message queue. The server collects results and calculates the mean.

## Build and Run

Run everything with one command:

`gcc task1_pipe_fork.c -o task1_pipe_fork && gcc task2_shared_memory.c -o task2_shared_memory -lrt && gcc task3_pi_queue.c -o task3_pi_queue -lrt && ./task1_pipe_fork && ./task2_shared_memory && ./task3_pi_queue`

## Notes

All tasks use POSIX system calls and fork() for process creation. Each task demonstrates a different interprocess communication method: pipe, shared memory, and message queue.
