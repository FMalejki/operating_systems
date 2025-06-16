#ifndef COMMON_H
#define COMMON_H

#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <time.h>

//stale
#define QUEUE_SIZE 10
#define TEXT_LEN 10

#define SHM_KEY ftok("/tmp", 'S')
#define SEM_KEY ftok("/tmp", 'M')

//job
typedef struct {
    char text[TEXT_LEN];
} print_job;

//kolejka
typedef struct {
    print_job jobs[QUEUE_SIZE];
    int head;
    int tail;
} print_queue;

//semafory
enum {
    MUTEX = 0,
    SLOTS = 1,
    ITEMS = 2
};

//semun dla semctl - na macu reinitailization, na linuxie brak
#if !defined(__APPLE__) && !defined(__MACH__)
union semun {
    int val;
    struct semid_ds *buf;
    unsigned short *array;
};
#endif

// zmniejsza semafor o 1
int down(int semid, int semnum) {
    struct sembuf op = {semnum, -1, 0};
    return semop(semid, &op, 1);
}

// zwieksza semafor o 1
int up(int semid, int semnum) {
    struct sembuf op = {semnum, +1, 0};
    return semop(semid, &op, 1);
}

#endif
