#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stdbool.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <signal.h>

#define MESSAGE_SIZE 1024

volatile sig_atomic_t flag = 1;

typedef enum {
    INIT = 1,
    IDENTIFIER = 2,
    MESSAGE_TEXT = 3,
    DISCONNECT = 4
} message_type_t;

typedef struct {
    long mtype;
    message_type_t type;
    int identifier;
    char text[MESSAGE_SIZE];
} message_t;

void sigint_handle(int sig){
    flag = 0;
}

int main(){
    signal(SIGINT, sigint_handle);

    key_t client_key = ftok(".", 'A' + (getpid() % 26));
    int client_queue_id = msgget(client_key, IPC_CREAT | 0666);
    if( client_queue_id == -1){
        perror("msgget");
        exit(1);
    }
    int server_queue_id = msgget(1234, 0);
    if( server_queue_id == -1){
        perror("msgget");
        exit(1);
    }

    char client_key_str[32];
    sprintf(client_key_str, "%d", client_key);

    message_t message_init = {
        .mtype = 1,
        .type = INIT,
        .identifier = -1
    };
    strcpy(message_init.text, client_key_str);

    if( msgsnd(server_queue_id, &message_init, sizeof(message_t) - sizeof(long), 0) == -1){
        printf("Could not connect to the server!");
        exit(1);
    }

    message_t recieved_from_init;
    int server_id;
    msgrcv(client_queue_id, &recieved_from_init, sizeof(message_t) - sizeof(long), 0, 0);
    if(recieved_from_init.identifier){
        server_id = recieved_from_init.identifier;
        printf("Connected to server!");
    }
    else{
        printf("Error");
        exit(1);
    }

    pid_t listener_pid = fork();
    if(listener_pid == 0){
        while(flag){
            message_t message_res;
            int bytes = msgrcv(client_queue_id, &message_res, sizeof(message_t) - sizeof(long), 0, IPC_NOWAIT);
            if(bytes == -1){
                continue;
            }

            switch (message_res.type){
                case MESSAGE_TEXT:
                    printf("Recieved from %d, Message: %s", message_res.identifier, message_res.text);
                    break;
                default:
                    printf("Unknown");
            }
        }
    } else {
        char* buffer = NULL;
        size_t len = 0;
        while(flag){
            printf("Enter message: \n");
            int txt_size = getline(&buffer, &len, stdin);
            if(txt_size == -1){
                perror("getline");
                exit(1);
            }
            if(txt_size > 0 && buffer[txt_size-1] == '\n'){
                buffer[txt_size-1] = '\0';
            }
            message_t message_send = {
                .identifier = server_id,
                .mtype = 1,
                .type = MESSAGE_TEXT,
            };
            strncpy(message_send.text, buffer, MESSAGE_SIZE);
            if (msgsnd(server_queue_id, &message_send, sizeof(message_t) - sizeof(long), 0) == -1)
                perror("msgsnd");
            free(buffer);
            buffer = NULL;
        }

        message_t ending_message = {
            .mtype = 1,
            .identifier = server_id,
            .type = DISCONNECT
        };
        msgsnd(server_queue_id, &ending_message, sizeof(message_t) - sizeof(long), 0);

        msgctl(client_queue_id, IPC_RMID, NULL);
    } 
}