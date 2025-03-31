#include <signal.h>
#include <stdio.h>
#include <spawn.h>
#include <bits/sigaction.h>

void obsluga(int signum){
    printf("obsluga sygnalu\n");
}

int main(int argc, char *argv[]){
    if(argc != 2){
        printf("bad structure");
        return 1;
    }
    if(argv == "none"){

    }else if(argv == "ignore"){

    }else if(argv == "handler"){
        signal(SIGUSR1, obsluga);
    }else if(argv == "mask"){
        sigset_t newmask, oldmask, set;
        sigemptyset(&newmask);
        sigaddset(&newmask, SIGUSR1);
        sigprocmask(SIG_BLOCK, &newmask, &oldmask);
        sigpending(&set);
        if(sigismember(&set, SIGUSR1) == 1){
            printf("SIGUSR1 oczekuje\n");
        }
    }else{
        printf("Bad argument");
        return 1;
    }
    raise(SIGUSR1);


    raise(SIGUSR1);

    raise(SIGUSR1);
    while(1);
    return 0;
}