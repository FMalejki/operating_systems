#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <unistd.h>

void obsluga(int signum) {
    printf("Odebrano sygnał SIGUSR1 w handlerze.\n");
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s [none|ignore|handler|mask]\n", argv[0]);
        return 1;
    }

    if (strcmp(argv[1], "none") == 0) {
    }
    else if (strcmp(argv[1], "ignore") == 0) {
        signal(SIGUSR1, SIG_IGN);
    }
    else if (strcmp(argv[1], "handler") == 0) {
        signal(SIGUSR1, obsluga);
    }
    else if (strcmp(argv[1], "mask") == 0) {
        sigset_t block_set;
        sigemptyset(&block_set);
        sigaddset(&block_set, SIGUSR1);
        
        if (sigprocmask(SIG_BLOCK, &block_set, NULL) < 0) {
            perror("sigprocmask");
            return 1;
        }
    }
    else {
        fprintf(stderr, "Error: %s\n", argv[1]);
        return 1;
    }

    raise(SIGUSR1);

    if (strcmp(argv[1], "mask") == 0) {
        sigset_t pending;
        sigpending(&pending);
        if (sigismember(&pending, SIGUSR1)) {
            printf("SIGUSR1 is waiting.\n");
        } else {
            printf("SIGUSR1 is not waiting.\n");
        }
    }

    return 0;
}
