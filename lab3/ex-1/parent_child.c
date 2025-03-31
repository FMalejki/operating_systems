#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

int global = 0;

int main(int argc, char *argv[]){
    if(argc != 2){
        fprintf(stderr, "Usage: %s <directory_path>\n", argv[0]);
        return 1;
    }

    printf("program name: %s\n", argv[0]);

    int local = 0;

    pid_t pid = fork();
    if (pid < 0) {
        perror("fork");
        return 1;
    }
    if(pid == 0){
        printf("child process\n");
        global++;
        local++;
        printf("child pid = %d, parent pid = %d\n", getpid(), getppid());
        printf("child's local = %d, child's global = %d\n", local, global);
        execl("/bin/ls", "ls", argv[1], NULL);
        perror("execl");
        return 1;
    } else {
        int child_exit_code;
        wait(&child_exit_code);
        printf("parent process\n");
        printf("parent's pid = %d, child's pid = %d\n", getpid(), pid);
        printf("child exit code: %d\n", WEXITSTATUS(child_exit_code));
        printf("parent local = %d, parent global = %d\n", local, global);
        return child_exit_code;
    }

    return 0;
}
