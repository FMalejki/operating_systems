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
    int semid = semget(SEM_KEY, 3, 0);
    enum {
        MUTEX,
        SLOTS,
        ITEMS
    };
    if(shmid == -1 || semid == -1){
        perror("shmget || semget");
    }
    print_queue *queue = (print_queue *)shmat(shmid, NULL, 0);
    if(queue == (void *)-1){
        perror("shmat");
    }   

    while(1){
        struct sembuf operation = {ITEMS, -1, 0};
        semop(semid, &operation, 1);

        struct sembuf operation = {MUTEX, -1, 0};
        semop(semid, &operation, 1);

        print_job job = queue->jobs[queue->head];
        queue->head = (queue->head + 1) % QUEUE_SIZE;

        struct sembuf operation = {MUTEX, 1, 0};
        semop(semid, &operation, 1);

        struct sembuf operation = {SLOTS, 1, 0};
        semop(semid, &operation, 1);

        printf("[Printer %d] Printing job:\n", getpid());
        for (int i = 0; i < TEXT_LEN; i++) {
            putchar(job.text[i]);
            putchar('\n');
            fflush(stdout);
            sleep(1);
        }

    }
    return 0;
}