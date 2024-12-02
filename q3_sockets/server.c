#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/select.h>
#include <fcntl.h>
#include <pthread.h>

#define PORT 8080
#define MAX_CLIENTS 4
#define BUFFER_SIZE 1024

char *usernames[4] = {
        "davie504",
        "john19",
        "kdot1975",
        "d4v1d"};

int online[4] = {0};



char *verify_user(int socket)
{
    char confirm_user[BUFFER_SIZE] = "Enter your username:";
    char *username = malloc(BUFFER_SIZE);
    send(socket, confirm_user, strlen(confirm_user), 0);
    printf("Verifying user...\n");
    while (1)
    {
        int valread = recv(socket, username, BUFFER_SIZE, 0);
        if (valread <= 0) {
            printf("Client disconnected.\n");
            break;
        }
        for (int i = 0; i < MAX_CLIENTS; i++)
        {
            if (strcmp(usernames[i], username) == 0)
            {
                printf("Login successful\n");
                online[i] = 1;
                return username;
            }
        }

        if (strncmp(username, "exit", 4) == 0) {
            printf("Client ended communication.\n");
            break;
        }

        char message[BUFFER_SIZE] = "Login unsuccessful. Enter valid username or type \"exit\" to quit";
        send(socket, message, strlen(message), 0);
        
    }
    return NULL;
}


int main() {
    int server_fd, new_socket, max_sd, activity, sd, client_socket[MAX_CLIENTS];;
    struct sockaddr_in address;
    fd_set readfds;
    char buffer[BUFFER_SIZE];
    char *user;

    // Initialize all client sockets to 0
    for (int i = 0; i < MAX_CLIENTS; i++) {
        client_socket[i] = 0;
    }

    // Create server socket
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        perror("Socket failed");
        exit(EXIT_FAILURE);
    }

    // Set server socket to non-blocking mode
    int flags = fcntl(server_fd, F_GETFL, 0);
    fcntl(server_fd, F_SETFL, flags | O_NONBLOCK);

    // Bind the server socket to the address and port
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);

    if (bind(server_fd, (struct sockaddr*)&address, sizeof(address)) < 0) {
        perror("Bind failed");
        exit(EXIT_FAILURE);
    }

    // Listen for incoming connections
    if (listen(server_fd, 3) < 0) {
        perror("Listen failed");
        exit(EXIT_FAILURE);
    }

    printf("Server listening on port %d\n", PORT);

    while (1) {
        // Clear and set up socket set
        FD_ZERO(&readfds);
        FD_SET(server_fd, &readfds);
        max_sd = server_fd;

        // Add client sockets to the set
        for (int i = 0; i < MAX_CLIENTS; i++) {
            sd = client_socket[i];
            if (sd > 0) FD_SET(sd, &readfds);
            if (sd > max_sd) max_sd = sd;
        }

        // Wait for activity
        activity = select(max_sd + 1, &readfds, NULL, NULL, NULL);

        // Check for new connection
        if (FD_ISSET(server_fd, &readfds)) {
            socklen_t addrlen = sizeof(address);
            new_socket = accept(server_fd, (struct sockaddr*)&address, &addrlen);

            if (new_socket >= 0) 
            {
                // Set new client socket to non-blocking mode
                flags = fcntl(new_socket, F_GETFL, 0);
                fcntl(new_socket, F_SETFL, flags | O_NONBLOCK);

                printf("New connection, socket fd is %d, ip is: %s, port: %d\n",
                       new_socket, inet_ntoa(address.sin_addr), ntohs(address.sin_port));

                user = verify_user(new_socket);
                if (user)
                {
                // Add new socket to client array
                show_online_users();
                for (int i = 0; i < MAX_CLIENTS; i++)
                {
                    if (client_socket[i] == 0)
                    {
                        client_socket[i] = new_socket;
                        break;
                    }
                }
                }

                
            }
        }


        // Check each client for incoming data
        for (int i = 0; i < MAX_CLIENTS; i++) {
            sd = client_socket[i];
            if (FD_ISSET(sd, &readfds)) {
                memset(buffer, 0, BUFFER_SIZE);
                int valread = recv(sd, buffer, BUFFER_SIZE, 0);

                if (valread <= 0) {
                    // Client disconnected
                    close(sd);
                    client_socket[i] = 0;
                    printf("Client disconnected, socket fd is %d\n", sd);
                } else {
                    // Check for "end" message
                    if (strncmp(buffer, "end", 3) == 0) {
                        close(sd);
                        client_socket[i] = 0;
                        printf("Client sent end message, disconnecting socket fd %d\n", sd);
                    } else {
                        // Read two numbers from the buffer, compute sum, and send back to client
                        int receipient;
                        char message[BUFFER_SIZE];
                        sscanf(buffer, "%d %s", &receipient, &message);
                        sprintf(buffer, "%s: %s", user, message);
                        send(client_socket[receipient], buffer, strlen(buffer), 0);
                    }
                }
            }
        }
    }

    return 0;
}
