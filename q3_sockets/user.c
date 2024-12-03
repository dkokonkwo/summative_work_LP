#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <pthread.h>

#define PORT 8080
#define BUFFER_SIZE 1024

int sock = 0; // Global socket for communication
int verified = 0;

void *receive_messages(void *arg) 
{
    char buffer[BUFFER_SIZE];
    while (1) {
        memset(buffer, 0, BUFFER_SIZE);
        int valread = recv(sock, buffer, BUFFER_SIZE, 0);
        if (valread > 0)
        {
            buffer[valread] = '\0';
            printf("%s\n", buffer);
            // Display the received message
        }
        else if (valread == 0)
        {
            printf("\nServer disconnected.\n");
            break;
        }
        else
        {
            perror("Error receiving message");
            break;
        }
        
    }
    return NULL;
}

void *send_messages(void *arg)
{
    char message[BUFFER_SIZE];
    while(1)
    {
        memset(message, 0, BUFFER_SIZE);

        // Getting message from server or chat
        if (verified)
            printf("You: ");
        fgets(message, BUFFER_SIZE, stdin);
        send(sock, message, strlen(message), 0);

        if (strncmp(message, "end", 3) == 0)
        {
            printf("You've ended your chat.\n");
            break;
        }
    }
    return NULL;
}

int user_verify(int socket) {
    char server_response[BUFFER_SIZE];
    char my_username[BUFFER_SIZE];
    while (1) {
        int valread = recv(socket, server_response, BUFFER_SIZE, 0);
        if (valread <= 0) {
            printf("Server disconnected.\n");
            return 0;
        }
        server_response[valread] = '\0';

        if (strncmp(server_response, "logged in", 9) == 0) {
            printf("%s\n", server_response);
            printf("Verified\n");
            return 1;
        }

        printf("%s", server_response);

        fgets(my_username, BUFFER_SIZE, stdin);
        my_username[strcspn(my_username, "\n")] = '\0';

        send(socket, my_username, strlen(my_username), 0);

        if (strcmp(my_username, "exit") == 0) {
            printf("Client chose to exit during login.\n");
            return 0;
        }
    }
}

int main() {
    struct sockaddr_in serv_addr;
    char buffer[BUFFER_SIZE] = {0};
    int success = 0;
    // pthread_t recv_thread;

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
    if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
        printf("Connection failed\n");
        return -1;
    }

    printf("Connected to the server.\n");

    while (!success)
    {
        success = user_verify(sock);
    }

    pthread_t send_thread, recv_thread;
    pthread_create(&recv_thread, NULL, receive_messages, NULL);
    pthread_create(&send_thread, NULL, send_messages, NULL);
    
    pthread_join(recv_thread, NULL);
    pthread_join(send_thread, NULL);


    close(sock);
    return 0;
}
