#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Bad usage");
        return 1;
    }

    int n = atoi(argv[1]);
    if (n <= 0) {
        fprintf(stderr, "Process number must be postitive\n");
        return 1;
    }

    for (int i = 0; i < n; i++) {
        pid_t pid = fork();
        if (pid < 0) {
            perror("fork"); //fork error
            return 1;
        }
        if (pid == 0) {
            printf("parent pid = %d,  child pid = %d\n", getppid(), getpid());
            return 0;
        }
    }

    for (int i = 0; i < n; i++) {
        wait(NULL);
    }

    printf("%d\n", n);
    return 0;
}