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
    int socket;
    char id[32];
    int active;
    time_t last_alive;
} Client;

Client clients[MAX_CLIENTS];
pthread_mutex_t clients_mutex = PTHREAD_MUTEX_INITIALIZER;
int server_socket;

// do wszystkich
void broadcast_message(const char* sender_id, const char* message, int exclude_socket) {
    pthread_mutex_lock(&clients_mutex);
    for (int i = 0; i < MAX_CLIENTS; i++) {
        if (clients[i].active && clients[i].socket != exclude_socket) {
            char buffer[BUFFER_SIZE];
            time_t now = time(NULL);
            struct tm *t = localtime(&now);
            char time_str[64];
            strftime(time_str, sizeof(time_str), "%Y-%m-%d %H:%M:%S", t);
            snprintf(buffer, sizeof(buffer), "[%s | %s]: %s", sender_id, time_str, message);
            send(clients[i].socket, buffer, strlen(buffer), 0);
        }
    }
    pthread_mutex_unlock(&clients_mutex);
}

// send alive
void* send_alive_pings(void* arg) {
    while (1) {
        sleep(10);

        pthread_mutex_lock(&clients_mutex);
        time_t now = time(NULL);

        for (int i = 0; i < MAX_CLIENTS; i++) {
            if (clients[i].active) {
                send(clients[i].socket, "ALIVE\n", 6, 0);

                if (now - clients[i].last_alive > 15) {
                    printf("Client %s timed out. Removing...\n", clients[i].id);
                    close(clients[i].socket);
                    clients[i].active = 0;
                }
            }
        }

        pthread_mutex_unlock(&clients_mutex);
    }
    return NULL;
}

// polaczenie
void* handle_client(void* arg) {
    sleep(10);
    int client_socket = *((int*)arg);
    char buffer[BUFFER_SIZE];
    char client_id[32];
    int read_size;

    // Receive client id
    if ((read_size = recv(client_socket, client_id, sizeof(client_id), 0)) <= 0) {
        perror("recv");
        close(client_socket);
        return NULL;
    }
    client_id[read_size] = '\0';

    // dodanie do tablicy klientow
    pthread_mutex_lock(&clients_mutex);
    for (int i = 0; i < MAX_CLIENTS; i++) {
        if (!clients[i].active) {
            clients[i].socket = client_socket;
            strcpy(clients[i].id, client_id);
            clients[i].active = 1;
            clients[i].last_alive = time(NULL);
            break;
        }
    }
    pthread_mutex_unlock(&clients_mutex);

    while ((read_size = recv(client_socket, buffer, sizeof(buffer), 0)) > 0) {
        buffer[read_size] = '\0';
        if (strncmp(buffer, "IMOK", 4) == 0) {
            // potwierdzenie ze alive
            pthread_mutex_lock(&clients_mutex);
            for (int i = 0; i < MAX_CLIENTS; i++) {
                if (clients[i].socket == client_socket) {
                    clients[i].last_alive = time(NULL);
                    break;
                }
            }
            pthread_mutex_unlock(&clients_mutex);
            continue;
        } else if (strncmp(buffer, "LIST", 4) == 0) {
            pthread_mutex_lock(&clients_mutex);
            // lista klientow
            for (int i = 0; i < MAX_CLIENTS; i++) {
                if (clients[i].active) {
                    char list_message[BUFFER_SIZE];
                    snprintf(list_message, sizeof(list_message), "Client: %s\n", clients[i].id);
                    send(client_socket, list_message, strlen(list_message), 0);
                }
            }
            pthread_mutex_unlock(&clients_mutex);
        } else if (strncmp(buffer, "2ALL", 4) == 0) {
            // funkcja wiadomosc do wszystkich
            broadcast_message(client_id, buffer + 5, client_socket);
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
                    send(clients[i].socket, private_message, strlen(private_message), 0);
                    break;
                }
            }
            pthread_mutex_unlock(&clients_mutex);
        } else if (strncmp(buffer, "STOP", 4) == 0) {
            break;
        }
    }

    close(client_socket);

    pthread_mutex_lock(&clients_mutex);
    for (int i = 0; i < MAX_CLIENTS; i++) {
        if (clients[i].socket == client_socket) {
            clients[i].active = 0;
            break;
        }
    }
    pthread_mutex_unlock(&clients_mutex);

    return NULL;
}

void handle_sigint(int sig) {
    // ctrl + c (sigint)
    printf("\nShutting down server...\n");
    pthread_mutex_lock(&clients_mutex);
    for (int i = 0; i < MAX_CLIENTS; i++) {
        if (clients[i].active) {
            send(clients[i].socket, "Server is shutting down...\n", strlen("Server is shutting down...\n"), 0);
            send(clients[i].socket, "STOP", strlen("STOP"), 0);
            close(clients[i].socket);
        }
    }
    pthread_mutex_unlock(&clients_mutex);

    close(server_socket);
    exit(EXIT_SUCCESS);
}

int main(int argc, char* argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Bad Usage!\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    signal(SIGINT, handle_sigint);

    //adresy
    struct sockaddr_in server_addr, client_addr;
    socklen_t client_addr_len = sizeof(client_addr);

    int port = atoi(argv[1]);

    // TCP IPv4 Creation
    if ((server_socket = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
        perror("socket");
        exit(EXIT_FAILURE);
    }

    server_addr.sin_family = AF_INET; //IPv4
    server_addr.sin_addr.s_addr = INADDR_ANY; //nasluchuje wszedzie
    server_addr.sin_port = htons(port); //port (porządek bajtow)

    // gniazdo do adresu i portu 
    if (bind(server_socket, (struct sockaddr*)&server_addr, sizeof(server_addr)) == -1) {
        perror("bind");
        close(server_socket);
        exit(EXIT_FAILURE);
    }

    // gniazdo w tryb nasluchuwania
    if (listen(server_socket, 10) == -1) {
        perror("listen");
        close(server_socket);
        exit(EXIT_FAILURE);
    }


    //pobiera przeksztalca i wypisuje server_ip
    char server_ip[INET_ADDRSTRLEN];
    getsockname(server_socket, (struct sockaddr*)&server_addr, &client_addr_len);
    inet_ntop(AF_INET, &server_addr.sin_addr, server_ip, sizeof(server_ip));
    printf("Server listening on port %d on ip %s\n", port, server_ip);

    // watek ktory co 10s wysyla ALIVE
    pthread_t alive_thread;
    pthread_create(&alive_thread, NULL, send_alive_pings, NULL);
    pthread_detach(alive_thread);


    while (1) {
        int client_socket;
        //akceptuje nowe polaczenia
        printf("%p \n",&client_socket);
        if ((client_socket = accept(server_socket, (struct sockaddr*)&client_addr, &client_addr_len)) == -1) {
            perror("accept");
            continue;
        }

        //dla kazdego klienta nowy watek
        pthread_t thread;
        pthread_create(&thread, NULL, handle_client, (void*)&client_socket);
        pthread_detach(thread); //dzialla niezaleznie
    }

    close(server_socket);
    return 0;
}