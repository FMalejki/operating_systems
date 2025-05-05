#include <stdio.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <semaphore.h>

#define ROZM_BLOKU 1024

int main(int argc, char **argv){
    int fd = shm_open("/name_pac", O_CREAT | O_RDWR, 0644);
    ftruncate(fd, ROZM_BLOKU);
    char *wsk = (char *) mmap(NULL, ROZM_BLOKU, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    printf("Odczytano z pamieci wspolnej: %s\n",wsk);
    munmap(wsk,  ROZM_BLOKU);
    shm_ulink("/nazwa_pam");
    return 0;
}