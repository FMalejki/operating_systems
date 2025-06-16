#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stdbool.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <signal.h>

#define MESSAGE_BUFFER_SIZE 1024
#define MAX_CLIENTS_COUNT 3

#define SERVER_QUEUE_KEY 1234

typedef enum {
    INIT = 1,
    IDENTIFIER = 2,
    MESSAGE_TEXT = 3,
    CLIENT_CLOSE = 4
} message_type_t;

typedef struct {
    long mtype;
    message_type_t type;
    int identifier;
    char text[MESSAGE_BUFFER_SIZE];
} message_t;

volatile sig_atomic_t flag = 0;

void SIGNAL_handler(int sig) {
    flag = 1;
}

#define MIN(a, b) ((a) < (b) ? (a) : (b))

int main() {
    signal(SIGINT, SIGNAL_handler);

    key_t client_key = ftok(".", getpid() & 0xFF);
    int client_qid = msgget(client_key, IPC_CREAT | 0666);
    if (client_qid == -1) {
        perror("msgget (client)");
        exit(1);
    }

    int server_qid = msgget(SERVER_QUEUE_KEY, 0);
    if (server_qid == -1) {
        perror("msgget (server)");
        exit(1);
    }

    message_t msg_init = {
        .mtype = 1,
        .type = INIT,
        .identifier = -1
    };
    //printf("sizeof(message_t) = %zu\n", sizeof(message_t));
    //printf("sizeof(long) = %zu\n", sizeof(long));
    //printf("server_qid = %d\n", server_qid);

    snprintf(msg_init.text, MESSAGE_BUFFER_SIZE, "%d", client_key);
    if (msgsnd(server_qid, &msg_init, sizeof(message_t) - sizeof(long), 0) == -1) {
        perror("msgsnd (init)");
        exit(1);
    }

    int to_parent_pipe[2];
    if (pipe(to_parent_pipe) < 0)
        perror("pipe");

    pid_t listener_pid = fork();
    if (listener_pid == 0) {
        // listener
        close(to_parent_pipe[0]);
        message_t msg;
        while (!flag) {
            ssize_t bytes = msgrcv(client_qid, &msg, sizeof(message_t) - sizeof(long), 0, 0);
            if (bytes == -1) continue;

            switch (msg.type) {
                case MESSAGE_TEXT:
                    printf("Received from id %d: %s\n", msg.identifier, msg.text);
                    break;
                case IDENTIFIER:
                    write(to_parent_pipe[1], &msg.identifier, sizeof(msg.identifier));
                    printf("Received assigned ID: %d\n", msg.identifier);
                    break;
                default:
                    printf("Unknown message type: %d\n", msg.type);
            }
        }
        exit(0);
    } else {
        // sender
        close(to_parent_pipe[1]);
        int identifier = -1;
        if (read(to_parent_pipe[0], &identifier, sizeof(identifier)) < 0) {
            perror("read (identifier)");
            exit(1);
        }

        char* buffer = NULL;
        size_t len = 0;
        while (!flag) {
            printf("Enter message: \n");
            ssize_t read = getline(&buffer , &len, stdin);
            if(read == -1){
                perror("getline");
                break;
            }
            if(read > 0 && buffer[read-1] == '\n'){
                buffer[read-1] = '\0';
            }
            message_t msg_send = {
                .mtype = 1,
                .type = MESSAGE_TEXT,
                .identifier = identifier
            };
            strncpy(msg_send.text, buffer, MESSAGE_BUFFER_SIZE - 1);
            msg_send.text[MESSAGE_BUFFER_SIZE - 1] = '\0';

            if (msgsnd(server_qid, &msg_send, sizeof(message_t) - sizeof(long), 0) == -1)
                perror("msgsnd (message)");

            free(buffer);
            buffer = NULL;

        }

        if (identifier != -1) {
            message_t msg_close = {
                .mtype = 1,
                .type = CLIENT_CLOSE,
                .identifier = identifier
            };
            msgsnd(server_qid, &msg_close, sizeof(message_t) - sizeof(long), 0);
        }

        msgctl(client_qid, IPC_RMID, NULL);
        exit(0);
    }

    return 0;
}
