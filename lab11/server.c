#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <signal.h>
#include <time.h>

#define MAX_CLIENTS 10
#define BUFFER_SIZE 1024

typedef struct {
    struct sockaddr_in addr;
    char id[32];
    int active;
    time_t last_alive;
} Client;

Client clients[MAX_CLIENTS];
pthread_mutex_t clients_mutex = PTHREAD_MUTEX_INITIALIZER;
int server_socket;

//porownanie klientow (ip i porty)
int compare_clients(struct sockaddr_in* a, struct sockaddr_in* b) {
    return a->sin_addr.s_addr == b->sin_addr.s_addr &&
           a->sin_port == b->sin_port;
}

//wyslanie wiadomosci do wszystkich
void broadcast_message(const char* sender_id, const char* message, struct sockaddr_in* exclude_addr) {
    pthread_mutex_lock(&clients_mutex);
    for (int i = 0; i < MAX_CLIENTS; i++) {
        if (clients[i].active &&
            !compare_clients(&clients[i].addr, exclude_addr)) {
            char buffer[BUFFER_SIZE];
            time_t now = time(NULL);
            struct tm *t = localtime(&now);
            char time_str[64];
            strftime(time_str, sizeof(time_str), "%Y-%m-%d %H:%M:%S", t);
            snprintf(buffer, sizeof(buffer), "[%s | %s]: %s", sender_id, time_str, message);
            sendto(server_socket, buffer, strlen(buffer), 0,
                   (struct sockaddr*)&clients[i].addr, sizeof(clients[i].addr));
        }
    }
    pthread_mutex_unlock(&clients_mutex);
}

//alive ping
void* send_alive_pings(void* arg) {
    while (1) {
        sleep(10);
        time_t now = time(NULL);

        pthread_mutex_lock(&clients_mutex);
        for (int i = 0; i < MAX_CLIENTS; i++) {
            if (clients[i].active) {
                sendto(server_socket, "ALIVE\n", 6, 0,
                       (struct sockaddr*)&clients[i].addr, sizeof(clients[i].addr));

                if (now - clients[i].last_alive > 15) {
                    printf("Client %s timed out. Removing...\n", clients[i].id);
                    clients[i].active = 0;
                }
            }
        }
        pthread_mutex_unlock(&clients_mutex);
    }
    return NULL;
}

int main(int argc, char* argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <port>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    int port = atoi(argv[1]);
    char buffer[BUFFER_SIZE];
    struct sockaddr_in server_addr, client_addr;
    socklen_t addr_len = sizeof(client_addr);

    //stworzenie socketu i bindowanie
    server_socket = socket(AF_INET, SOCK_DGRAM, 0);
    if (server_socket < 0) {
        perror("socket");
        exit(EXIT_FAILURE);
    }

    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port);
    server_addr.sin_addr.s_addr = INADDR_ANY;

    //bindowanie
    if (bind(server_socket, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        perror("bind");
        close(server_socket);
        exit(EXIT_FAILURE);
    }

    //uruchomienie watku z pingiem
    pthread_t ping_thread;
    pthread_create(&ping_thread, NULL, send_alive_pings, NULL);

    while (1) {
        int read_size = recvfrom(server_socket, buffer, sizeof(buffer) - 1, 0,
                                 (struct sockaddr*)&client_addr, &addr_len);
        if (read_size <= 0) continue;
        buffer[read_size] = '\0';

        pthread_mutex_lock(&clients_mutex);
        int known = 0, free_index = -1;
        //rozpoznanie klienta (czy znamy nadawce)
        for (int i = 0; i < MAX_CLIENTS; i++) {
            if (clients[i].active && compare_clients(&clients[i].addr, &client_addr)) {
                known = 1;
                if (strcmp(buffer, "IMOK") == 0) {
                    clients[i].last_alive = time(NULL);
                    pthread_mutex_unlock(&clients_mutex);
                    continue;
                }
                break;
            } else if (!clients[i].active && free_index == -1) {
                free_index = i;
            }
        }

        //rejestracja klientow
        if (!known) {
            if (free_index != -1) {
                clients[free_index].addr = client_addr;
                strncpy(clients[free_index].id, buffer, sizeof(clients[free_index].id));
                clients[free_index].active = 1;
                clients[free_index].last_alive = time(NULL);
            }
            pthread_mutex_unlock(&clients_mutex);
            continue;
        }

        //sprawdzenie ktory wyslal
        char client_id[32];
        for (int i = 0; i < MAX_CLIENTS; i++) {
            if (clients[i].active && compare_clients(&clients[i].addr, &client_addr)) {
                strncpy(client_id, clients[i].id, sizeof(client_id));
                clients[i].last_alive = time(NULL);
                break;
            }
        }
        pthread_mutex_unlock(&clients_mutex);

        //obsluga komend
        if (strncmp(buffer, "LIST", 4) == 0) {
            pthread_mutex_lock(&clients_mutex);
            for (int i = 0; i < MAX_CLIENTS; i++) {
                if (clients[i].active) {
                    char msg[BUFFER_SIZE];
                    snprintf(msg, sizeof(msg), "Client: %s\n", clients[i].id);
                    sendto(server_socket, msg, strlen(msg), 0,
                           (struct sockaddr*)&client_addr, addr_len);
                }
            }
            pthread_mutex_unlock(&clients_mutex);
        } else if (strncmp(buffer, "2ALL", 4) == 0) {
            broadcast_message(client_id, buffer + 5, &client_addr);
        } else if (strncmp(buffer, "2ONE", 4) == 0) {
            char target_id[32];
            sscanf(buffer + 5, "%s", target_id);
            char* msg = strchr(buffer + 5, ' ') + 1;
            pthread_mutex_lock(&clients_mutex);
            for (int i = 0; i < MAX_CLIENTS; i++) {
                if (clients[i].active && strcmp(clients[i].id, target_id) == 0) {
                    char private_message[BUFFER_SIZE];
                    time_t now = time(NULL);
                    struct tm *t = localtime(&now);
                    char time_str[64];
                    strftime(time_str, sizeof(time_str), "%Y-%m-%d %H:%M:%S", t);
                    snprintf(private_message, sizeof(private_message), "[%s to %s | %s]: %s", client_id, target_id, time_str, msg);
                    sendto(server_socket, private_message, strlen(private_message), 0,
                           (struct sockaddr*)&clients[i].addr, sizeof(clients[i].addr));
                    break;
                }
            }
            pthread_mutex_unlock(&clients_mutex);
        } else if (strncmp(buffer, "STOP", 4) == 0) {
            pthread_mutex_lock(&clients_mutex);
            for (int i = 0; i < MAX_CLIENTS; i++) {
                if (clients[i].active && compare_clients(&clients[i].addr, &client_addr)) {
                    clients[i].active = 0;
                    break;
                }
            }
            pthread_mutex_unlock(&clients_mutex);
        }
    }

    close(server_socket);
    return 0;
}
