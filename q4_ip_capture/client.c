#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <pthread.h>

#define PORT 8080
#define BUFFER_SIZE 1024
#define HOST "127.0.0.1"

int sockfd;  // Global variable for the server socket

void *receive_messages(void *arg) {
    char buffer[BUFFER_SIZE];
    while (1) {
        memset(buffer, 0, BUFFER_SIZE);

        // Receive message from server
        int valread = recv(sockfd, buffer, BUFFER_SIZE, 0);
        if (valread <= 0) {
            printf("Server disconnected.\n");
            break;
        }
        printf("Server: %s\n", buffer);

        // Check for "bye" to end the chat
        if (strncmp(buffer, "done", 4) == 0) {
            printf("Server ended the chat.\n");
            break;
        }

        buffer[valread] = '\0';
    }
    return NULL;
}

void *send_messages(void *arg) {
    char message[BUFFER_SIZE];
    while (1) {
        memset(message, 0, BUFFER_SIZE);

        // Get message from client user
        printf("Client: ");
        fgets(message, BUFFER_SIZE, stdin);
        send(sockfd, message, strlen(message), 0);

        // Check for "bye" to end the chat
        if (strncmp(message, "done", 4) == 0) {
            printf("Client ended the chat.\n");
            break;
        }
    }
    return NULL;
}

int main() {
    struct sockaddr_in server_addr;

    // Create socket
    if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("Socket creation error");
        exit(EXIT_FAILURE);
    }

    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);

    // Convert IPv4 and IPv6 addresses from text to binary form
    if (inet_pton(AF_INET, HOST, &server_addr.sin_addr) <= 0) {
        perror("Invalid address/ Address not supported");
        close(sockfd);
        exit(EXIT_FAILURE);
    }

    // Connect to the server
    if (connect(sockfd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        perror("Connection failed");
        close(sockfd);
        exit(EXIT_FAILURE);
    }
    printf("Connected to the server.\n");

    // Create threads for sending and receiving messages
    pthread_t send_thread, recv_thread;
    pthread_create(&send_thread, NULL, send_messages, NULL);
    pthread_create(&recv_thread, NULL, receive_messages, NULL);

    // Wait for both threads to finish
    pthread_join(send_thread, NULL);
    pthread_join(recv_thread, NULL);

    // Close the socket
    close(sockfd);
    return 0;
}
