#include <fcntl.h>
#include <sys/mman.h>
#include <stdio.h>

#define ROZM_BLOKU 1024

int main(int argc, char **argv){
    int fd = shm_open("/nazwa_pam", O_CREAT | O_RDWR, 0644);
    ftruncate(fd, ROZM_BLOKU);
    char *wsk = (char *) mmap(NULL, ROZM_BLOKU, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    strcpy(wsk, "dane do zapisania w pamieci wspolnej");
    munmap(wsk, ROZM_BLOKU);
    shm_unlink("/nazwa_pam");
    return 0;
}