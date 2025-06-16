#include "common.h"

int main() {
    srand(getpid());

    // shared memory oraz semafory
    int shmid = shmget(SHM_KEY, sizeof(print_queue), 0666);
    int semid = semget(SEM_KEY, 3, 0666);

    if (shmid == -1 || semid == -1) {
        perror("Accessing shared memory/semaphores");
        exit(1);
    }

    // podlaczenie do pam wspoldziel.
    print_queue *queue = (print_queue *)shmat(shmid, NULL, 0);
    if (queue == (void *)-1) {
        perror("shmat");
        exit(1);
    }

    while (1) {
        print_job job;
        for (int i = 0; i < TEXT_LEN; i++)
            job.text[i] = 'a' + rand() % 26;

        //czeka na wolne miejsce
        down(semid, SLOTS);
        //blokuje dostep do kolejki
        down(semid, MUTEX);

        queue->jobs[queue->tail] = job;
        queue->tail = (queue->tail + 1) % QUEUE_SIZE;
        printf("[User %d] Sent job: %.10s\n", getpid(), job.text);

        //zwalnianie mutexu
        up(semid, MUTEX);
        //sygnalizowanie ze mamuy nowy element
        up(semid, ITEMS);

        sleep(rand() % 5 + 1);
    }

    return 0;
}
