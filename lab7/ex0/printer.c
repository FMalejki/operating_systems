#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <time.h>


#define N 2               // users
#define M 2               // printers
#define QUEUE_SIZE 10     // queue size

// txt
typedef struct {
    char text[10];
} print_job;

// queue
typedef struct {
    print_job jobs[QUEUE_SIZE]; //queue
    int head; // first job in the queue
    int tail; // adding new jobs
    sem_t mutex; //synchro
    sem_t slots; //num of slots in queue (free)
    sem_t items; //num of items in queue
    //sem_t print_mutex;
} print_queue;

print_queue *queue;

void *user(void *arg) {
    int id = *(int *)arg;

    while (1) {
        // Wygeneruj zadanie
        print_job job;
        for (int i = 0; i < 10; i++) {
            job.text[i] = 'a' + rand() % 26;
        }

        // Dodaj do kolejki
        sem_wait(&queue->slots);//wait
        sem_wait(&queue->mutex);//block
        queue->jobs[queue->tail] = job;//add job
        queue->tail = (queue->tail + 1) % QUEUE_SIZE;//change tail
        printf("[User %d] Sent job: %.10s\n", id, job.text);
        sem_post(&queue->mutex);//unlock
        sem_post(&queue->items);//send

        //wait
        sleep(rand() % 5 + 1);
    }
    return NULL;
}

void *printer(void *arg) {
    int id = *(int *)arg;
    while (1) {
        // same as upper
        sem_wait(&queue->items);
        sem_wait(&queue->mutex);
        print_job job = queue->jobs[queue->head];
        queue->head = (queue->head + 1) % QUEUE_SIZE;
        sem_post(&queue->mutex);
        sem_post(&queue->slots);

        //sem_wait(&queue->print_mutex);
        //printf("[Printer %d] Printing job: \n", id);
        for (int i = 0; i < 10; i++) {
            putchar(job.text[i]);
            putchar('\n');
            fflush(stdout);
            sleep(1);
        }
        putchar('\n');
        fflush(stdout);
        //sem_post(&queue->print_mutex);
    }
    return NULL;
}


int main() {
    srand(time(NULL));
    pthread_t users[N], printers[M];
    int user_ids[N], printer_ids[M];

    // init shared memory
    queue = mmap(NULL, sizeof(print_queue),
                 PROT_READ | PROT_WRITE,
                 MAP_SHARED | MAP_ANONYMOUS, -1, 0);
    if (queue == MAP_FAILED) {
        perror("mmap");
        exit(1);
    }

    // queue init
    queue->head = 0;
    queue->tail = 0;
    sem_init(&queue->mutex, 1, 1);
    sem_init(&queue->slots, 1, QUEUE_SIZE);
    sem_init(&queue->items, 1, 0);

    // users
    for (int i = 0; i < N; i++) {
        user_ids[i] = i;
        pthread_create(&users[i], NULL, user, &user_ids[i]);
    }

    // printers
    for (int i = 0; i < M; i++) {
        printer_ids[i] = i;
        pthread_create(&printers[i], NULL, printer, &printer_ids[i]);
    }

    // never ends
    for (int i = 0; i < N; i++) {
        pthread_join(users[i], NULL);
    }
    for (int i = 0; i < M; i++) {
        pthread_join(printers[i], NULL);
    }

    return 0;
}
