#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>

#define PORT 8080
#define BUFFER_SIZE 1024

int main() {
    int sock = 0;
    struct sockaddr_in serv_addr;
    char buffer[BUFFER_SIZE] = {0};

    // Create socket
    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        printf("Socket creation error\n");
        return -1;
    }

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);

    // Convert IPv4 and IPv6 addresses from text to binary form
    if (inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr) <= 0) {
        printf("Invalid address/ Address not supported\n");
        return -1;
    }

    // Connect to server
    if (connect(sock, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) < 0) {
        printf("Connection failed\n");
        return -1;
    }

    printf("Connected to the server.");

    while (1) {
        printf("Use <receipent index> <message> to chat (or type end to quit): ");
        memset(buffer, 0, BUFFER_SIZE);
        fgets(buffer, BUFFER_SIZE, stdin);

        buffer[strcspn(buffer, "\n")] = '\0';

        // Check for "end" message to terminate connection
        if (strncmp(buffer, "end", 3) == 0)
        {
            send(sock, buffer, strlen(buffer), 0);
            printf("Ending connection.\n");
            break;
        }

        // Sending message to the server
        if (send(sock, buffer, strlen(buffer), 0) < 0)
        {
            perror("Failed to send message");
            break;
        }

        // Receive the response from the server
        memset(buffer, 0, BUFFER_SIZE);
        int valread = recv(sock, buffer, BUFFER_SIZE, 0);
        if (valread > 0)
        {
            printf("Server response: %s\n", buffer);
        }
        else if (valread == 0)
        {
            printf("Server disconnected.\n");
            break;
        }
        else
        {
            perror("Error receiving server response");
            break;
        }
    }

    // Close the socket
    close(sock);
    return 0;
}
