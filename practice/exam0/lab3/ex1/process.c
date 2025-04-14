#include <unistd.h>
#include <stdio.h>
#include <sys/wait.h>


int global = 0;

int main(int argc, char** argv){
    if(argc != 2)
        return 1;
    printf("%s", argv[0]);
    int local = 0;
    pid_t pid = fork();
    if(pid < 0){
        perror("fork");
        return 1;
    }
    if(pid == 0){
        printf("child process\n");
        local++;
        global++;
        printf("child pid = %d, parent pid = %d\n", getpid(), getppid());
        printf("child's local = %d, child's global = %d\n", local, global);
        execl("/bin/ls", "ls", argv[1], NULL);
        return 0;
    } else {
        int exit_code;
        wait(&exit_code);
        printf("parent process\n");
        printf("child pid = %d, parent pid = %d\n", pid, getppid());
        printf("child exit code: %d\n", exit_code);
        printf("parent's local = %d, parent's global = %d\n", local, global);
        return exit_code;
    }    

}