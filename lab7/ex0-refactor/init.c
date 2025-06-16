#include "common.h"

int main() {
    //tworzenie pamieci wspoldzielonej
    printf("Rozmiar segmentu: %lu bajtów\n", sizeof(print_queue));

    int shmid = shmget(SHM_KEY, sizeof(print_queue), IPC_CREAT | 0666);
    if (shmid == -1) {
        perror("shmget");
        exit(1);
    }

    // podlaczenie do pam. wspoldz
    print_queue *queue = (print_queue *)shmat(shmid, NULL, 0);
    if (queue == (void *)-1) {
        perror("shmat");
        exit(1);
    }

    queue->head = 0;
    queue->tail = 0;

    //tworzenie semaforow
    int semid = semget(SEM_KEY, 3, IPC_CREAT | 0666);
    if (semid == -1) {
        perror("semget");
        exit(1);
    }

    //inicjalizacja semaforow
    union semun arg;
    arg.val = 1;
    semctl(semid, MUTEX, SETVAL, arg);
    arg.val = QUEUE_SIZE;
    semctl(semid, SLOTS, SETVAL, arg);
    arg.val = 0;
    semctl(semid, ITEMS, SETVAL, arg);

    printf("System initialized.\n");

    return 0;
}
