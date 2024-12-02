#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <pthread.h>
#include <netdb.h>
#include <pthread.h>
#include <errno.h>

#define PORT 8080
#define BUFFER_SIZE 1024
#define HOST "127.0.0.1"
#define NETWORK_PREFIX "192.168.1"

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

void scan_network_and_send() {
    char ip[BUFFER_SIZE];
    char message[BUFFER_SIZE];
    struct sockaddr_in sa;
    socklen_t len = sizeof(sa);
    struct hostent *he;

    for (int i = 1; i <= 254; i++) {
        snprintf(ip, BUFFER_SIZE, "%s.%d", NETWORK_PREFIX, i);

        // Check if the IP is reachable via a socket connection
        sa.sin_family = AF_INET;
        sa.sin_port = htons(PORT);
        if (inet_pton(AF_INET, ip, &sa.sin_addr) <= 0) {
            continue; // Skip invalid IPs
        }

        // Try resolving hostname
        he = gethostbyaddr(&sa.sin_addr, sizeof(sa.sin_addr), AF_INET);
        if (he) {
            snprintf(message, BUFFER_SIZE, "IP: %s, Hostname: %s", ip, he->h_name);
        } else {
            snprintf(message, BUFFER_SIZE, "IP: %s, Hostname: Unknown", ip);
        }

        // Send the IP and hostname to the server
        send(sockfd, message, strlen(message), 0);
        printf("Sent: %s\n", message);
    }

    // Signal the server that the scan is done
    strcpy(message, "done");
    send(sockfd, message, strlen(message), 0);
    printf("Scan complete.\n");
}

void *send_messages(void *arg) {
    scan_network_and_send();
    return NULL;
}

int main()
{
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
