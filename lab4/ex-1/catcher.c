#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

volatile sig_atomic_t received_count = 0;
volatile sig_atomic_t mode = 0;
volatile sig_atomic_t sender_pid = 0;
volatile sig_atomic_t got_signal = 0;

void mode_1_behaviour() {
    printf("Received mode 1 command %d times\n", received_count);
}

void ctrlc_handler(int sig) {
    if (mode == 4) {
        printf("Wciśnięto CTRL+C\n");
    } else if (mode != 3) {
        exit(0);
    }
}

void handle_usr1(int sig, siginfo_t* info, void* context) {
    sender_pid = info->si_pid;
    mode = info->si_value.sival_int;
    got_signal = 1;
}

int main() {
    printf("Catcher PID: %d\n", getpid());

    struct sigaction act_usr1;
    act_usr1.sa_sigaction = handle_usr1;
    sigemptyset(&act_usr1.sa_mask);
    act_usr1.sa_flags = SA_SIGINFO;
    sigaction(SIGUSR1, &act_usr1, NULL);

    struct sigaction act_ctrlc;
    act_ctrlc.sa_handler = ctrlc_handler;
    sigemptyset(&act_ctrlc.sa_mask);
    act_ctrlc.sa_flags = 0;
    sigaction(SIGINT, &act_ctrlc, NULL);

    while (1) {
        pause();
        if (got_signal) {
            got_signal = 0;

            if (mode == 1) {
                received_count++;
                mode_1_behaviour();
            } else if (mode == 2) {
                for (int i = 1;; i++) {
                    printf("%d\n", i);
                    sleep(1);
                    if (mode != 2) break;
                }
            } else if (mode == 3) {
                signal(SIGINT, SIG_IGN);
                printf("Catcher will ignore Ctrl+C\n");
            } else if (mode == 4) {
                signal(SIGINT, ctrlc_handler);
                printf("Catcher will now print on Ctrl+C\n");
            } else if (mode == 5) {
                printf("Catcher shutting down.\n");
                exit(0);
            }

            kill(sender_pid, SIGUSR1);
        }
    }

    return 0;
}
