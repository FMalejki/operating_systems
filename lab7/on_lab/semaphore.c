#include <stdio.h>
#include <fcntl.h>
#include <semaphore.h>

int main(void){
    sem_t *id_sem = sem_open("nazwa_sem", O_CREAT, 0666);
    pid_t pid1 = fork();
    if(pid1 == 0){
        sleep(1);
        printf("czekam potomy\n"); fflush(stdout);
        sem_wait(id_sem);
        printf("wewnatrz potomny\n"); fflush(stdout);
        sleep(3);
        sem_post(id_sem);
        printf("po potomnym\n"); fflush(stdout);
        return 0;
    } else {
        printf("czekam glowny\n"); fflush(stdout);
        sem_wait(id_sem);
        printf("wewnatrz glowny\n"); fflush(stdout);
        sleep(2);
        sem_post(id_sem);
        printf("po glownym\n"); fflush(stdout);
    }
    sem_close(id_sem);
    return 0;
}