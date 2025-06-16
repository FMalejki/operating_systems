#include "common.h"

int main() {
    //wspoldzielona
    int shmid = shmget(SHM_KEY, sizeof(print_queue), 0666);
    //semafory
    int semid = semget(SEM_KEY, 3, 0666);

    if (shmid == -1 || semid == -1) {
        perror("Accessing shared memory/semaphores");
        exit(1);
    }

    //podlaczenie do pam wspoldziel.
    print_queue *queue = (print_queue *)shmat(shmid, NULL, 0);
    if (queue == (void *)-1) {
        perror("shmat");
        exit(1);
    }

    while (1) {
        //czeka na elementy
        down(semid, ITEMS);
        //blokuje dostep do kolejki
        down(semid, MUTEX);

        print_job job = queue->jobs[queue->head];
        queue->head = (queue->head + 1) % QUEUE_SIZE;

        //zwalnia mutex
        up(semid, MUTEX);
        //zwalnia miejsce
        up(semid, SLOTS);

        //druk
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
