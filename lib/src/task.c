#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <unistd.h>
#include <sys/select.h>
#include "defines.h"

#include "task.h"
#include "pipe.h"

void task_A(void) {
    //printf("task A \n");
    int value = 42;
    char buffer[BUF_SIZE];

    snprintf(buffer, sizeof(buffer), "%d", value);
    write_to_pipe(AB, buffer);

#ifdef DEBUG
    printf("task A: write: %d \n", value);
#endif
    usleep(100);

    exit(0);
}

void task_A_1(void) {
    int value = 42;
    char buffer[BUF_SIZE];

    snprintf(buffer, sizeof(buffer), "%d", value);

    write_to_pipe(AB_1, buffer);
    write_to_pipe(AB_2, buffer);
    write_to_pipe(AB_3, buffer);

    usleep(1000);

    exit(0);
}

void task_B(void) {
    char buffer[BUF_SIZE];

    // Read from pipe AB
    if (read_from_pipe(AB, buffer, BUF_SIZE)) {
        int value = atoi(buffer);
        value++;
        snprintf(buffer, sizeof(buffer), "%d", value);
#ifdef DEBUG
        printf("task B: read: %d \t write: %d \n", value - 1, value);
#endif
        // Write to pipe BC
        write_to_pipe(BC, buffer);

        usleep(100); // Avoid busy loop

        exit(0);
    }

    exit(1);
}

void task_B_1(void) {
    char buffer[BUF_SIZE];

    // Read from pipe AB
    if (read_from_pipe(AB_1, buffer, BUF_SIZE)) {
        int value = atoi(buffer);
        value++;
        snprintf(buffer, sizeof(buffer), "%d", value);

        // Write to pipe BC
        write_to_pipe(BC_1, buffer);

        //usleep(1000); // Avoid busy loop
    }

    exit(0);
}

void task_B_2(void) {
    char buffer[BUF_SIZE];

    // Read from pipe AB
    if (read_from_pipe(AB_2, buffer, BUF_SIZE)) {
        int value = atoi(buffer);
        value++;
        snprintf(buffer, sizeof(buffer), "%d", value);

        // Write to pipe BC
        write_to_pipe(BC_2, buffer);

        //usleep(1000); // Avoid busy loop
    }

    exit(0);
}

void task_B_3(void) {
    char buffer[BUF_SIZE];

    // Read from pipe AB
    if (read_from_pipe(AB_3, buffer, BUF_SIZE)) {
        int value = atoi(buffer);
        value++;
        snprintf(buffer, sizeof(buffer), "%d", value);

        // Write to pipe BC
        write_to_pipe(BC_3, buffer);

        //usleep(1000); // Avoid busy loop
    }

    exit(0);
}

void task_C(void) {
    char buffer[BUF_SIZE];

    if (read_from_pipe(BC, buffer, BUF_SIZE)) {
        int value = atoi(buffer);
#ifdef DEBUG
        printf("task C: read: %d \n", value);
#endif
        usleep(100);
        exit(0);
    }
    printf("C crashed \n");
    exit(1);
}

void task_C_1(void) {
    char buffer[BUF_SIZE];

    if (read_from_pipe(CD, buffer, BUF_SIZE)) {
        int value = atoi(buffer);
    }

    exit(0);
}

void voter(void) {
    char buffer1[BUF_SIZE], buffer2[BUF_SIZE], buffer3[BUF_SIZE];
    char outputBuffer[BUF_SIZE];
    bool read1, read2, read3;

    read1 = read_from_pipe(BC_1, buffer1, BUF_SIZE);
    read2 = read_from_pipe(BC_1, buffer2, BUF_SIZE);
    read3 = read_from_pipe(BC_1, buffer3, BUF_SIZE);

    if (read1 && read2 && strcmp(buffer1, buffer2) == 0) {
        strncpy(outputBuffer, buffer1, BUF_SIZE);
    } else if (read1 && read3 && strcmp(buffer1, buffer3) == 0) {
        strncpy(outputBuffer, buffer1, BUF_SIZE);
    } else if (read2 && read3 && strcmp(buffer2, buffer3) == 0) {
        strncpy(outputBuffer, buffer2, BUF_SIZE);
    } else if (read1) {
        strncpy(outputBuffer, buffer1, BUF_SIZE);
    } else if (read2) {
        strncpy(outputBuffer, buffer2, BUF_SIZE);
    } else if (read3) {
        strncpy(outputBuffer, buffer3, BUF_SIZE);
    } else {
        // If no valid data is read, handle the error or set a default value
        strncpy(outputBuffer, "Nop", BUF_SIZE);
    }

    write_to_pipe(CD, outputBuffer);
}