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

void handle_exit(int sig) {
    char stop_msg[] = "STOP";
    sendto(client_socket, stop_msg, strlen(stop_msg), 0,
           (struct sockaddr*)&server_addr, sizeof(server_addr));
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

    if ((client_socket = socket(AF_INET, SOCK_DGRAM, 0)) == -1) {
        perror("socket");
        exit(EXIT_FAILURE);
    }

    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(server_port);
    if (inet_pton(AF_INET, server_ip, &server_addr.sin_addr) <= 0) {
        perror("inet_pton");
        close(client_socket);
        exit(EXIT_FAILURE);
    }

    signal(SIGINT, handle_exit);

    // Wysyłamy ID klienta (np. "ID <id>")
    char id_msg[BUFFER_SIZE];
    snprintf(id_msg, sizeof(id_msg), "ID %s", client_id);
    sendto(client_socket, id_msg, strlen(id_msg), 0,
           (struct sockaddr*)&server_addr, sizeof(server_addr));

    char buffer[BUFFER_SIZE];
    fd_set read_fds;

    while (1) {
        FD_ZERO(&read_fds);
        FD_SET(STDIN_FILENO, &read_fds);
        FD_SET(client_socket, &read_fds);

        if (select(client_socket + 1, &read_fds, NULL, NULL, NULL) == -1) {
            perror("select");
            break;
        }

        if (FD_ISSET(STDIN_FILENO, &read_fds)) {
            if (fgets(buffer, sizeof(buffer), stdin) != NULL) {
                sendto(client_socket, buffer, strlen(buffer), 0,
                       (struct sockaddr*)&server_addr, sizeof(server_addr));
            }
        }

        if (FD_ISSET(client_socket, &read_fds)) {
            ssize_t recv_len = recvfrom(client_socket, buffer, sizeof(buffer) - 1, 0, NULL, NULL);
            if (recv_len > 0) {
                buffer[recv_len] = '\0';

                if (strcmp(buffer, "ALIVE\n") == 0) {
                    char imok[] = "IMOK";
                    sendto(client_socket, imok, strlen(imok), 0,
                           (struct sockaddr*)&server_addr, sizeof(server_addr));
                    continue;
                }

                printf("%s", buffer);
            }
        }
    }

    handle_exit(0);
    return 0;
}
