#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <unistd.h>
#include <signal.h>

#define MESSAGE_SIZE 1024

typedef enum {
    INIT = 1,
    IDENTIFIER = 2,
    MESSAGE_TEXT = 3,
    DISCONNECT = 4
} message_type_t;

typedef struct{
    long mtype;
    message_type_t type;
    int identifier;
    char text[MESSAGE_SIZE];
} message_t;

volatile sig_atomic_t flag = 1;

void sigint_handle(int sig){
    flag = 0;
}

int main(){
    signal(SIGINT, sigint_handle);
    int MAX_CLIENTS = 10;
    int server_queue_id = msgget(1234, IPC_CREAT | 0666);
    if( server_queue_id == -1){
        perror("msgget");
        exit(1);
    }
    int client_arr[MAX_CLIENTS];
    for(int i = 0; i < MAX_CLIENTS; i++){
        client_arr[i] = -1;
    }

    message_t message;
    while(flag){
        ssize_t bytes = msgrcv(server_queue_id, &message, sizeof(message_t) - sizeof(long), 0, 0);
        if(bytes == -1) return 1;
        
        switch(message.type){
            case INIT: {
                int id = 0;
                while (id < MAX_CLIENTS && client_arr[id] != -1) id++;
                if(id == MAX_CLIENTS){
                    printf("MAX CLIENTS REACHED!");
                    continue;
                }
                key_t client_key = atoi(message.text);
                int client_queue_id = msgget(client_key, 0);
                if (client_queue_id == -1) {
                    perror("msgget");
                    continue;
                }
                client_arr[id] = client_queue_id;
                message_t response = {
                    .mtype = 1,
                    .type = IDENTIFIER,
                    .identifier = id
                };
                msgsnd(client_queue_id, &response, sizeof(message_t) - sizeof(long), 0);
                printf("New Client Connected with id: %d\n", id);
                break;
            }
            case MESSAGE_TEXT: {
                for(int i = 0; i < MAX_CLIENTS; i++){
                    if(client_arr[i] != -1){
                        msgsnd(client_arr[i], &message, sizeof(message_t) - sizeof(long), 0);
                        printf("Message propagated to %d\n", i);
                    }
                }
                break;
            }
            case DISCONNECT: {
                int id = message.identifier;
                client_arr[id] = -1;
                printf("Client Disconnected, ID: %d\n", id);
                break;
            }
            default: 
                printf("Whhaaat??");

        }
    }

    printf("Server Gracefully Stopping...");

    msgctl(server_queue_id, IPC_RMID, NULL);

    return 0;

}