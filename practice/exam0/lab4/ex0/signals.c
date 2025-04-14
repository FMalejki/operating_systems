#include <stdio.h>
#include <string.h>
#include <signal.h>
#include <unistd.h>

void handler(int sig){
    printf("Handling signal %d\n", sig);
}

int main(int argc, char** argv){
    if(argc != 2){
        fprintf(stderr, "Usage: %s <none|ignore|handler|mask>\n", argv[0]);
        return 1;
    }

    if(strcmp(argv[1], "none") == 0){
        printf("No special handling for SIGUSR1.\n");
    }
    else if(strcmp(argv[1], "ignore") == 0){
        signal(SIGUSR1, SIG_IGN);
        printf("Ignoring SIGUSR1.\n");
    }
    else if(strcmp(argv[1], "handler") == 0){
        signal(SIGUSR1, handler);
        printf("Setting custom handler for SIGUSR1.\n");
    }
    else if(strcmp(argv[1], "mask") == 0){
        sigset_t sigset;
        sigemptyset(&sigset);
        sigaddset(&sigset, SIGUSR1);
        sigprocmask(SIG_BLOCK, &sigset, NULL);
        printf("Blocking SIGUSR1.\n");
    }
    else {
        fprintf(stderr, "Unknown option: %s\n", argv[1]);
        return 1;
    }

    raise(SIGUSR1);

    if(strcmp(argv[1], "mask") == 0){
        sigset_t pending;
        sigpending(&pending);
        if(sigismember(&pending, SIGUSR1)) {
            printf("SIGUSR1 is pending.\n");
        } else {
            printf("SIGUSR1 is not pending.\n");
        }
    }

    return 0;
}
