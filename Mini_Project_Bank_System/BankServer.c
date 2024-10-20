#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <pthread.h>

#define PORT 8080
#define MAX_CLIENTS 5
#define BUFFER_SIZE 1024

void *handle_client(void *client_socket) {
    int sock = *(int *)client_socket;
    free(client_socket);  // Prevent memory leaks
    char buffer[BUFFER_SIZE] = {0};
    char username[50], password[50], role[50];

    // Read the entire credentials string from the client
    recv(sock, buffer, sizeof(buffer), 0);

    // Split the received data using '#' as a delimiter
    char *token = strtok(buffer, "#");
    if (token) strcpy(username, token);
    token = strtok(NULL, "#");
    if (token) strcpy(password, token);
    token = strtok(NULL, "#");
    if (token) strcpy(role, token);
    
    
    if (username) printf("Username Received: %s\n", username);
    if (password) printf("Password Received: %s\n", password);
    if (role) printf("Role Received: %s\n", role);

    // Authenticate user
    if (strcmp(username, "")!=0 && strcmp(password, "")!=0 && strcmp(role, "")!=0){
    if (authenticate_user(username, password, role)) {
        send(sock, "Login successful!", strlen("Login successful!"), 0);
        int exit_flag = 0;
        while (!exit_flag) {
            char action[10] = {0};
            recv(sock, action, sizeof(action), 0);

            if (strcmp(action, "exit") == 0) {
                send(sock, "Exiting...", strlen("Exiting..."), 0);
                exit_flag = 1;
            } else {
                if (strcmp(role, "Admin") == 0) {
                    admin_login(sock);
                } else if (strcmp(role, "Employee") == 0) {
                    employee_login(sock, atoi(username));
                } else if (strcmp(role, "Manager") == 0) {
                    manager_login(sock, atoi(username));
                } else if (strcmp(role, "Customer") == 0) {
                    customer_login(sock, atoi(username));
                } else {
                    send(sock, "Invalid role!", strlen("Invalid role!"), 0);
                }
            }
        }
    } else {
        send(sock, "Login failed!", strlen("Login failed!"), 0);
    }
    }
    close(sock);
    return NULL;
}

int main() {
    int server_fd, *client_socket;
    struct sockaddr_in address;
    int addrlen = sizeof(address);

    // Create socket
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }

    // Bind socket
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);

    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0) {
        perror("Bind failed");
        close(server_fd);
        exit(EXIT_FAILURE);
    }

    // Listen for connections
    if (listen(server_fd, MAX_CLIENTS) < 0) {
        perror("Listen failed");
        close(server_fd);
        exit(EXIT_FAILURE);
    }

    printf("Server listening on port %d...\n", PORT);

    while (1) {
        client_socket = malloc(sizeof(int));
        if ((*client_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t *)&addrlen)) < 0) {
            perror("Accept failed");
            free(client_socket);
            continue;
        }

        printf("Connected to a client.\n");

        pthread_t thread_id;
        if (pthread_create(&thread_id, NULL, handle_client, client_socket) != 0) {
            perror("Failed to create thread");
            free(client_socket);
        }
        pthread_detach(thread_id);  // Automatically release thread resources when done
    }

    //close(server_fd);
    return 0;
}
