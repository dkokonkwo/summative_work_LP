#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <pthread.h>

#define PORT 8080
#define MAX_CLIENTS 4
#define BUFFER_SIZE 1024

char *usernames[MAX_CLIENTS] = {
    "davie504",
    "john19",
    "kdot1975",
    "d4v1d"};

int online[MAX_CLIENTS] = {0};
int client_sockets[MAX_CLIENTS] = {0};
pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;

void *client_handler(void *arg);
char *verify_user(int client_socket);
void show_online_users(int client_socket);

int main()
{
    int server_fd, new_socket;
    struct sockaddr_in address;
    pthread_t threads[MAX_CLIENTS];
    int addrlen = sizeof(address);

    // Create server socket
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0)
    {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }

    // Bind the server socket to the address and port
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);

    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0)
    {
        perror("Bind failed");
        close(server_fd);
        exit(EXIT_FAILURE);
    }

    // Listen for incoming connections
    if (listen(server_fd, 3) < 0)
    {
        perror("Listen failed");
        close(server_fd);
        exit(EXIT_FAILURE);
    }

    printf("Chat server started on port %d\n", PORT);

    while (1)
    {
        new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t *)&addrlen);
        if (new_socket < 0)
        {
            perror("Accept failed");
            continue;
        }

        printf("New connection, socket fd is %d, ip is: %s, port: %d\n",
               new_socket, inet_ntoa(address.sin_addr), ntohs(address.sin_port));

        // Create a thread for the new client
        for (int i = 0; i < MAX_CLIENTS; i++)
        {
            if (client_sockets[i] == 0)
            {
                if (pthread_create(&threads[i], NULL, client_handler, &new_socket) != 0)
                {
                    perror("Thread creation failed");
                }
                break;
            }
        }
    }

    close(server_fd);
    return 0;
}

void *client_handler(void *arg)
{
    int client_socket = *(int *)arg;
    char buffer[BUFFER_SIZE];
    char *username = verify_user(client_socket);

    if (!username)
    {
        close(client_socket);
        pthread_exit(NULL);
    }

    for (int i = 0; i < MAX_CLIENTS; i++)
    {
        if (strcmp(usernames[i], username) == 0 && client_sockets[i] == 0)
        {
            client_sockets[i] = client_socket;
            printf("User logged in: %s\n", username);
            break;
        }
        else if (strcmp(usernames[i], username) == 0 && client_sockets[i] != 0)
        {
            printf("User already logged in.\n");
            break;
        }
    }

    show_online_users(client_socket);

    while (1)
    {
        memset(buffer, 0, BUFFER_SIZE);
        int valread = recv(client_socket, buffer, BUFFER_SIZE, 0);
        if (valread <= 0)
        {
            printf("Client disconnected: %s\n", username);
            pthread_mutex_lock(&lock);
            for (int i = 0; i < MAX_CLIENTS; i++)
            {
                if (strcmp(usernames[i], username) == 0)
                {
                    online[i] = 0;
                    client_sockets[i] = 0;
                    break;
                }
            }
            pthread_mutex_unlock(&lock);
            free(username);
            close(client_socket);
            pthread_exit(NULL);
        }

        buffer[valread] = '\0';

        // Parse and handle the message
        int recipient;
        char message[BUFFER_SIZE];
        char *msg_start = strchr(buffer, ' ');
        if (msg_start && sscanf(buffer, "%d", &recipient) == 1)
        {
            msg_start++;
            pthread_mutex_lock(&lock);
            if (recipient >= 0 && recipient < MAX_CLIENTS && online[recipient] && client_sockets[recipient] > 0)
            {
                snprintf(message, BUFFER_SIZE, "%s: %s", username, msg_start);
                send(client_sockets[recipient], message, strlen(message), 0);
                send(client_socket, "Message sent\n", strlen("Message sent\n"), 0);
            }
            else
            {
                send(client_socket, buffer, strlen(buffer), 0);
                send(client_socket, "Invalid recipient or user offline\n", strlen("Invalid recipient or user offline\n"), 0);
            }
            pthread_mutex_unlock(&lock);
        }
        else if (strncmp(message, "show online", 11) == 0)
        {
            show_online_users(client_socket);
        }
        else
        {
            send(client_socket, "Invalid message format. Use <recipient index> <message>\n",
                 strlen("Invalid message format. Use <recipient index> <message>\n"), 0);
        }
    }
}

char *verify_user(int client_socket)
{
    char *username = malloc(BUFFER_SIZE);
    if (!username)
    {
        perror("Malloc failed");
        return NULL;
    }

    char prompt[BUFFER_SIZE] = "Enter your username: ";
    send(client_socket, prompt, strlen(prompt), 0);

    while (1)
    {
        memset(username, 0, BUFFER_SIZE);
        int valread = recv(client_socket, username, BUFFER_SIZE, 0);
        if (valread <= 0)
        {
            free(username);
            return NULL;
        }

        username[valread] = '\0'; // Remove newline
        pthread_mutex_lock(&lock);
        for (int i = 0; i < MAX_CLIENTS; i++)
        {
            if (strcmp(usernames[i], username) == 0)
            {
                if (online[i] == 1)
                {
                    send(client_socket, "User already logged in. Try a different username.\n",
                         strlen("User already logged in. Try a different username.\n"), 0);
                }
                else
                {
                    online[i] = 1;
                    send(client_socket, "logged in\n", strlen("logged in\n"), 0);
                    pthread_mutex_unlock(&lock);
                    return username;
                }
            }
        }
        pthread_mutex_unlock(&lock);
        send(client_socket, username, strlen(username), 0);
        printf("wha: %s\n", username);
        send(client_socket, "Invalid username. Try again.\n", strlen("Invalid username. Try again.\n"), 0);
    }
}

void show_online_users(int client_socket)
{
    char buffer[BUFFER_SIZE] = "Online users:\n";
    char temp[BUFFER_SIZE];
    pthread_mutex_lock(&lock);
    for (int i = 0; i < MAX_CLIENTS; i++)
    {
        if (online[i])
        {
            snprintf(temp, sizeof(temp), "%d: %s\n", i, usernames[i]);
            strncat(buffer, temp, sizeof(buffer) - strlen(buffer) - 1);
        }
    }
    pthread_mutex_unlock(&lock);
    send(client_socket, buffer, strlen(buffer), 0);
}
