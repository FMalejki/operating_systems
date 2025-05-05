#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <unistd.h>
#include <signal.h>

#define MESSAGE_BUFFER_SIZE 1024
#define MAX_CLIENTS_COUNT 3

#define SERVER_QUEUE_KEY 1234

volatile sig_atomic_t flag = 0;

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

void SIGNAL_handler(int sig) {
    flag = 1;
}

int main() {
    signal(SIGINT, SIGNAL_handler);

    int server_qid = msgget(SERVER_QUEUE_KEY, IPC_CREAT | 0666);
    if (server_qid == -1) {
        perror("msgget (server)");
        exit(1);
    }

    int client_queues[MAX_CLIENTS_COUNT];
    for (int i = 0; i < MAX_CLIENTS_COUNT; i++)
        client_queues[i] = -1;

    message_t msg;
    while (!flag) {
        ssize_t bytes = msgrcv(server_qid, &msg, sizeof(message_t) - sizeof(long), 0, 0);
        if (bytes == -1) continue;

        switch (msg.type) {
            case INIT: {
                int id = 0;
                while (id < MAX_CLIENTS_COUNT && client_queues[id] != -1) id++;

                if (id == MAX_CLIENTS_COUNT) {
                    printf("Max clients reached\n");
                    continue;
                }

                key_t client_key = atoi(msg.text);
                int client_qid = msgget(client_key, 0);
                if (client_qid == -1) {
                    perror("msgget (client)");
                    continue;
                }

                client_queues[id] = client_qid;

                message_t response = {
                    .mtype = 1,
                    .type = IDENTIFIER,
                    .identifier = id
                };
                msgsnd(client_qid, &response, sizeof(message_t) - sizeof(long), 0);
                printf("New client connected with id: %d\n", id);
                break;
            }
            case MESSAGE_TEXT: {
                for (int i = 0; i < MAX_CLIENTS_COUNT; i++) {
                    if (i != msg.identifier && client_queues[i] != -1) {
                        msgsnd(client_queues[i], &msg, sizeof(message_t) - sizeof(long), 0);
                    }
                }
                break;
            }
            case CLIENT_CLOSE: {
                int id = msg.identifier;
                if (id >= 0 && id < MAX_CLIENTS_COUNT && client_queues[id] != -1) {
                    client_queues[id] = -1;
                    printf("Client with id: %d disconnected\n", id);
                }
                break;
            }
            default:
                printf("Unknown message type\n");
        }
    }

    printf("Server shutting down...\n");

    msgctl(server_qid, IPC_RMID, NULL);

    return 0;
}
