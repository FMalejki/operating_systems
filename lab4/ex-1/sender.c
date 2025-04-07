#define _POSIX_C_SOURCE 200809L
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <stdbool.h>

volatile sig_atomic_t received_confirmation = 0;

void handle_confirmation(int sig) {
    received_confirmation = 1;
    printf("Sender: received confirmation from catcher.\n");
}

int main(int argc, char* argv[]) {
    if (argc != 3) {
        printf("Bad usage!");
        return 1;
    }

    pid_t catcher_pid = atoi(argv[1]);
    int mode = atoi(argv[2]);

    struct sigaction act;
    act.sa_handler = handle_confirmation;
    sigemptyset(&act.sa_mask);
    act.sa_flags = 0;
    sigaction(SIGUSR1, &act, NULL);

    union sigval value;
    value.sival_int = mode;
    if (sigqueue(catcher_pid, SIGUSR1, value) == -1) {
        perror("sigqueue failed");
        return 1;
    }

    sigset_t mask;
    sigemptyset(&mask);
    while (!received_confirmation) {
        sigsuspend(&mask);
    }

    return 0;
}
