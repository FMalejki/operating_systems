#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

int main(int argc, char** argv){
    if(argc != 2)
        return 1;
    int num = atoi(argv[1]);
    for(int i = 0; i < num; i++){
        pid_t pid = fork();
        if(pid < 0){
            printf("processses: %d", i);
            perror("fork");
            return 1;
        }
        if(pid == 0){
            printf("parent pid: %d, child pid: %d \n", getppid(), getpid());
            return 0;
        }

    }
    for (int i = 0; i < num; i++) {
        wait(NULL);
    }
    printf("processes: %d", num);
    return 0;
}