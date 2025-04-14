#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

volatile sig_atomic_t sender_pid;
volatile sig_atomic_t mode;
volatile sig_atomic_t got_signal;
int received_count = 0;

void usr1_handler(int sig, siginfo_t* siginfo, void* context){
    sender_pid = siginfo->si_pid;
    mode = siginfo->si_value.sival_int;
    got_signal = 1;
}

void sigint_handler(int sig){
    if (mode == 4) {
        printf("Wciśnięto CTRL+C\n");
    } else if (mode != 3) {
        exit(0);
    }
}

int main(int argc, char** argv){
    printf("catcher pid: %d", getpid());

    struct sigaction handleusr1;
    handleusr1.sa_sigaction = usr1_handler;
    sigemptyset(&handleusr1.sa_mask);
    handleusr1.sa_flags = SA_SIGINFO;
    sigaction(SIGUSR1, &handleusr1, NULL);

    while(1){
        pause();
        if (got_signal){
            got_signal = 0;
            if(mode == 1){
                received_count++;
                printf("Received mode 1 command %d times\n", received_count);
            }
            else if(mode == 2){
                int i = 1;
                while(1){
                    printf("%d",i);
                    sleep(1);
                    if(mode != 2)
                        break;
                    i++;
                }
            }
            else if(mode == 3){
                signal(SIGINT, SIG_IGN);
                printf("Catcher will ignore Ctrl+C\n");
            }
            else if(mode == 4){
                signal(SIGINT, sigint_handler);
                printf("Catcher will now print on Ctrl+C\n");
            }
            else if(mode == 5){
                printf("Catcher shutting down.\n");
                exit(0);
            }
            kill(sender_pid, SIGUSR1);
        }
    }
    return 0;
}