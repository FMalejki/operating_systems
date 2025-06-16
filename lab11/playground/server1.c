#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <pthread.h>
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

int addr_equal(struct sockaddr_in *a, struct sockaddr_in *b) {
    return (a->sin_family == b->sin_family) &&
           (a->sin_port == b->sin_port) &&
           (a->sin_addr.s_addr == b->sin_addr.s_addr);
}

void add_or_update_client(struct sockaddr_in *client_addr, const char *id) {
    pthread_mutex_lock(&clients_mutex);
    time_t now = time(NULL);
    for (int i = 0; i < MAX_CLIENTS; i++) {
        if (clients[i].active && addr_equal(&clients[i].addr, client_addr)) {
            clients[i].last_alive = now;
            pthread_mutex_unlock(&clients_mutex);
            return;
        }
    }
    // Dodaj nowego klienta
    for (int i = 0; i < MAX_CLIENTS; i++) {
        if (!clients[i].active) {
            clients[i].addr = *client_addr;
            strncpy(clients[i].id, id, sizeof(clients[i].id) - 1);
            clients[i].id[sizeof(clients[i].id) - 1] = '\0';
            clients[i].active = 1;
            clients[i].last_alive = now;
            printf("Added client %s\n", clients[i].id);
            break;
        }
    }
    pthread_mutex_unlock(&clients_mutex);
}

void broadcast_message(const char* sender_id, const char* message, struct sockaddr_in* exclude_addr) {
    pthread_mutex_lock(&clients_mutex);
    time_t now = time(NULL);
    char buffer[BUFFER_SIZE];
    struct tm *t = localtime(&now);
    char time_str[64];
    strftime(time_str, sizeof(time_str), "%Y-%m-%d %H:%M:%S", t);
    snprintf(buffer, sizeof(buffer), "[%s | %s]: %s", sender_id, time_str, message);

    for (int i = 0; i < MAX_CLIENTS; i++) {
        if (clients[i].active && (!exclude_addr || !addr_equal(&clients[i].addr, exclude_addr))) {
            sendto(server_socket, buffer, strlen(buffer), 0,
                   (struct sockaddr*)&clients[i].addr, sizeof(clients[i].addr));
        }
    }
    pthread_mutex_unlock(&clients_mutex);
}

void* send_alive_pings(void* arg) {
    while (1) {
        sleep(10);
        pthread_mutex_lock(&clients_mutex);
        time_t now = time(NULL);
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

    if ((server_socket = socket(AF_INET, SOCK_DGRAM, 0)) == -1) {
        perror("socket");
        exit(EXIT_FAILURE);
    }

    struct sockaddr_in server_addr = {0};
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(port);

    if (bind(server_socket, (struct sockaddr*)&server_addr, sizeof(server_addr)) == -1) {
        perror("bind");
        close(server_socket);
        exit(EXIT_FAILURE);
    }

    pthread_t alive_thread;
    pthread_create(&alive_thread, NULL, send_alive_pings, NULL);
    pthread_detach(alive_thread);

    printf("UDP server listening on port %d\n", port);

    char buffer[BUFFER_SIZE];
    struct sockaddr_in client_addr;
    socklen_t addr_len = sizeof(client_addr);

    while (1) {
        ssize_t recv_len = recvfrom(server_socket, buffer, sizeof(buffer) - 1, 0,
                                    (struct sockaddr*)&client_addr, &addr_len);
        if (recv_len == -1) {
            perror("recvfrom");
            continue;
        }
        buffer[recv_len] = '\0';

        // Przykład protokołu:
        // Pierwsza wiadomość od klienta to jego ID
        // Inne wiadomości mogą być np. "IMOK", "2ALL <msg>", "STOP"

        if (strncmp(buffer, "ID ", 3) == 0) {
            add_or_update_client(&client_addr, buffer + 3);
        } else if (strncmp(buffer, "IMOK", 4) == 0) {
            pthread_mutex_lock(&clients_mutex);
            for (int i = 0; i < MAX_CLIENTS; i++) {
                if (clients[i].active && addr_equal(&clients[i].addr, &client_addr)) {
                    clients[i].last_alive = time(NULL);
                    break;
                }
            }
            pthread_mutex_unlock(&clients_mutex);
        } else if (strncmp(buffer, "2ALL ", 5) == 0) {
            // Znajdź klienta po adresie i wyślij do wszystkich
            pthread_mutex_lock(&clients_mutex);
            char sender_id[32] = "unknown";
            for (int i = 0; i < MAX_CLIENTS; i++) {
                if (clients[i].active && addr_equal(&clients[i].addr, &client_addr)) {
                    strncpy(sender_id, clients[i].id, sizeof(sender_id));
                    break;
                }
            }
            pthread_mutex_unlock(&clients_mutex);

            broadcast_message(sender_id, buffer + 5, &client_addr);
        } else if (strncmp(buffer, "STOP", 4) == 0) {
            pthread_mutex_lock(&clients_mutex);
            for (int i = 0; i < MAX_CLIENTS; i++) {
                if (clients[i].active && addr_equal(&clients[i].addr, &client_addr)) {
                    clients[i].active = 0;
                    printf("Client %s disconnected\n", clients[i].id);
                    break;
                }
            }
            pthread_mutex_unlock(&clients_mutex);
        }
    }

    close(server_socket);
    return 0;
}
