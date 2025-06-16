#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <signal.h>
#include <sys/select.h>

#define BUFFER_SIZE 1024

int client_socket;
struct sockaddr_in server_addr;

//obsluga sigint
void handle_exit() {
    sendto(client_socket, "STOP", 4, 0, (struct sockaddr*)&server_addr, sizeof(server_addr));
    close(client_socket);
    exit(EXIT_SUCCESS);
}

int main(int argc, char* argv[]) {
    if (argc != 4) {
        fprintf(stderr, "Usage: %s <id> <server_ip> <server_port>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    char* client_id = argv[1];
    char* server_ip = argv[2];
    int server_port = atoi(argv[3]);

    //tworzenie udp (gniazdo)
    if ((client_socket = socket(AF_INET, SOCK_DGRAM, 0)) == -1) {
        perror("socket");
        exit(EXIT_FAILURE);
    }

    //konfiguracja adresu srv
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(server_port);
    //konwersja ip txt na bin
    if (inet_pton(AF_INET, server_ip, &server_addr.sin_addr) <= 0) {
        perror("inet_pton");
        exit(EXIT_FAILURE);
    }

    //wysylanie id
    sendto(client_socket, client_id, strlen(client_id), 0, (struct sockaddr*)&server_addr, sizeof(server_addr));

    signal(SIGINT, handle_exit);

    char buffer[BUFFER_SIZE];
    fd_set read_fds;

    while (1) {
        //fd_set config
        FD_ZERO(&read_fds);
        FD_SET(STDIN_FILENO, &read_fds);
        FD_SET(client_socket, &read_fds);

        //blokada az cos sie wydarzy
        if (select(client_socket + 1, &read_fds, NULL, NULL, NULL) == -1) {
            perror("select");
            break;
        }

        //wejscie 
        if (FD_ISSET(STDIN_FILENO, &read_fds)) {
            if (fgets(buffer, sizeof(buffer), stdin) != NULL) {
                sendto(client_socket, buffer, strlen(buffer), 0, (struct sockaddr*)&server_addr, sizeof(server_addr));
            }
        }

        //wiadomosci od srv
        if (FD_ISSET(client_socket, &read_fds)) {
            socklen_t addr_len = sizeof(server_addr);
            int read_size = recvfrom(client_socket, buffer, sizeof(buffer) - 1, 0,
                                     (struct sockaddr*)&server_addr, &addr_len);
            if (read_size > 0) {
                buffer[read_size] = '\0';

                if (strcmp(buffer, "ALIVE\n") == 0) {
                    sendto(client_socket, "IMOK", 4, 0, (struct sockaddr*)&server_addr, sizeof(server_addr));
                    continue;
                }

                printf("%s", buffer);
            }
        }
    }

    handle_exit();
    return 0;
}
