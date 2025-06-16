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
    int shmid = shmget(SHM_KEY, sizeof(print_queue), IPC_CREAT | 0666);
    if( shmid == -1){
        perror("shmid");
    }
    print_queue *queue = (print_queue *)shmat(shmid, NULL, 0);
    if(queue == (void *)-1){
        perror("shmat");
    }
    queue->head = 0;
    queue->tail = 0;

    int semid = semget(SEM_KEY, 3, IPC_CREAT | 0666);
    if(semid == -1){
        perror("semget");
    }

    enum {
        MUTEX,
        SLOTS,
        ITEMS
    };

    union semun arg;
    arg.val = 1;
    semctl(semid, MUTEX, SETVAL, arg);
    arg.val = QUEUE_SIZE;
    semctl(semid, SLOTS, SETVAL, arg);
    arg.val = 0;
    semctl(semid, ITEMS, SETVAL, arg);

    print("initialized");

    return 0;
}