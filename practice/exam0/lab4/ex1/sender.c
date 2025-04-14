#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <stdbool.h>

volatile sig_atomic_t signal_recieved = 0;

void handle_signal(){
    signal_recieved = 1;
    printf("signal recieved back");
}

int main(int argc, char* argv[]) {
    if (argc != 3) {
        printf("Bad usage!");
        return 1;
    }
    pid_t catcher_pid = atoi(argv[2]);
    int mode = atoi(argv[2]);
    signal(SIGUSR1, handle_signal);

    union sigval value;
    value.sival_int = mode;
    if (kill(catcher_pid, SIGUSR1) == -1) {
        perror("sigqueue failed");
        return 1;
    }

    sigset_t mask;
    sigemptyset(&mask);
    while (!signal_recieved) {
        sigsuspend(&mask);
    }
}