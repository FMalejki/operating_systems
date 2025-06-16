#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <time.h>

#define QUEUE_SIZE 10
#define TEXT_LEN 10

#define SHM_KEY ftok("/tmp", 'S')
#define SEM_KEY ftok("/tmp", 'M')

typedef struct {
    char text[TEXT_LEN];
} print_job;

//kolejka
typedef struct {
    print_job jobs[QUEUE_SIZE];
    int head;
    int tail;
} print_queue;

int main(){
    int shmid = shmget(SHM_KEY, sizeof(print_queue), 0666);
    int semid = semget(SEM_KEY, 3, 0666);

    if(shmid == -1 || semid == -1){
        perror("shmget || semget");
    }

    print_queue *queue = (print_queue *)shmat(shmid, NULL, 0);
    if(queue == (void *)-1){
        perror("shmat");
    }

    enum {
        MUTEX,
        SLOTS,
        ITEMS
    };

    while(1){
        print_job job;
        for(int i = 0; i < TEXT_LEN; i++){
            job.text[i] = 'a' + rand() % 26;
        }

        struct sembuf operation = {SLOTS,-1,0};
        semop(semid, &operation, 1);

        struct sembuf operation = {MUTEX,-1,0};
        semop(semid, &operation, 1);

        queue->jobs[queue->tail] = job;
        queue->tail = (queue->tail + 1) % QUEUE_SIZE;
        printf("[User %d] Sent job: %.10s\n", getpid(), job.text);


        struct sembuf operation = {MUTEX,+1,0};
        semop(semid, &operation, 1);

        struct sembuf operation = {ITEMS,+1,0};
        semop(semid, &operation, 1);

    }
;}