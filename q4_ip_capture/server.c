#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <pthread.h>

#define PORT 8080
#define BUFFER_SIZE 1024

int new_socket;  // Global variable for the client socket
/**
 * receice_info: receive ip addresses and hostnames from client and logs it in a file
 * arg: arguments
 * Return: void pointer
 */
void *receive_info(void *arg) {
    char *filename = "network_devices.txt";
    char buffer[BUFFER_SIZE];
    FILE *file;
    file = fopen(filename, "a");
    if (!file)
    {
        perror("Could not open file");
        return NULL;
    }
    while (1)
    {
        memset(buffer, 0, BUFFER_SIZE);

        // Receive message from client
        int valread = recv(new_socket, buffer, BUFFER_SIZE, 0);
        if (valread <= 0) {
            printf("Client disconnected.\n");
            break;
        }

        if (strncmp(buffer, "done", 4) == 0) {
            printf("Client ended communication.\n");
            break;
        }

        printf("New device: %s\n", buffer);

        if (fprintf(file, "%s\n", buffer) < 0)
        {
            perror("Could not add new device");
        }
        buffer[valread] = '\0';
    } 
    fclose(file);
    return NULL;
}

void *send_messages(void *arg) {
    char message[BUFFER_SIZE];
    while (1) {
        memset(message, 0, BUFFER_SIZE);

        // Send message to client
        printf("Server: ");
        fgets(message, BUFFER_SIZE, stdin);
        send(new_socket, message, strlen(message), 0);

        if (strncmp(message, "close", 5) == 0) {
            printf("Server ended communication.\n");
            break;
        }
    }
    return NULL;
}

int main() {
    int server_fd;
    struct sockaddr_in address;
    int addrlen = sizeof(address);

    // Create socket
    server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd == -1) {
        perror("Socket failed");
        exit(EXIT_FAILURE);
    }

    // Bind socket to the port
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);

    if (bind(server_fd, (struct sockaddr *)&address, addrlen) < 0) {
        perror("Bind failed");
        close(server_fd);
        exit(EXIT_FAILURE);
    }

    // Listen for incoming connections
    if (listen(server_fd, 3) < 0) {
        perror("Listen failed");
        close(server_fd);
        exit(EXIT_FAILURE);
    }
    printf("Server is listening on port %d...\n", PORT);

    // Accept an incoming connection
    new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t*)&addrlen);
    if (new_socket < 0) {
        perror("Accept failed");
        close(server_fd);
        exit(EXIT_FAILURE);
    }
    printf("Connection established with client.\n");

    // Create threads for sending and receiving messages
    pthread_t send_thread, recv_thread;
    pthread_create(&send_thread, NULL, send_messages, NULL);
    pthread_create(&recv_thread, NULL, receive_info, NULL);

    // Wait for both threads to finish
    pthread_join(send_thread, NULL);
    pthread_join(recv_thread, NULL);

    // Close the socket
    close(new_socket);
    close(server_fd);
    return 0;
}
