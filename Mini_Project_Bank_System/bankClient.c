#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include "bankRole.h"

#define PORT 8080
#define MAXLINE 1024

void handle_admin_login(int client_socket) {
    char buffer[1024];

    // Wait for the password prompt from the server
    recv(client_socket, buffer, sizeof(buffer), 0);
    printf("Server: %s", buffer);  // Display prompt

    // Get admin password from user
    memset(buffer, 0, sizeof(buffer));  // Clear buffer
    admin_login(client_socket);
    // printf("Enter Admin Password: ");
    // scanf("%s", buffer);

    // // Send password to the server
    // send(client_socket, buffer, strlen(buffer), 0);

    // // Wait for response from the server
    // recv(client_socket, buffer, sizeof(buffer), 0);
    // printf("Server: %s", buffer);  // Display server's response
}


int main() {
    int sock = 0;
    struct sockaddr_in serv_addr;
    char buffer[MAXLINE] = {0};
    char username[MAXLINE], password[MAXLINE], role[MAXLINE];
    int exit_flag = 0;

    // Create socket
    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        printf("\nSocket creation error\n");
        return -1;
    }

    // Server address setup
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);

    // Convert IPv4 and IPv6 addresses from text to binary form
    if (inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr) <= 0) {
        printf("\nInvalid address/ Address not supported\n");
        return -1;
    }

    // Connect to server
    if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
        printf("\nConnection Failed\n");
        return -1;
    }

    while (!exit_flag) {
        int role_selected = 0;

        // Role selection
        printf("Select your role:\n");
        printf("1. Customer\n");
        printf("2. Employee\n");
        printf("3. Manager\n");
        printf("4. Admin\n");
        printf("5. Exit\n");
        printf("Enter your choice: ");
        int choice;
        scanf("%d", &choice);

        // Handle role selection
        switch (choice) {
            case 1:
                strcpy(role, "Customer");
                role_selected = 1;
                break;
            case 2:
                strcpy(role, "Employee");
                role_selected = 1;
                break;
            case 3:
                strcpy(role, "Manager");
                role_selected = 1;
                break;
            case 4:
                strcpy(role, "Admin");
                role_selected = 1;
                break;
            case 5:
                printf("Exiting...\n");
                exit_flag = 1;
                continue;
            default:
                printf("Invalid choice. Please try again.\n");
                continue;
        }

        if (role_selected) {
            // Input username and password
            if (strcmp(role, "Customer")==0){
                printf("UserID: ");
                scanf("%s", username);
                
            }else if (strcmp(role, "Admin")==0){
                printf("Admin ID: ");
                scanf("%s", username);
            }else if (strcmp(role, "Manager")==0){
                printf("Manager ID: ");
                scanf("%s", username);
            }else{
                printf("Employee ID: ");
                scanf("%s", username);
            }
            printf("Password: ");
            scanf("%s", password);

            // Send credentials with delimiter '#'
            char credentials[MAXLINE];
            snprintf(credentials, sizeof(credentials), "%s#%s#%s\n", username, password, role);
            send(sock, credentials, strlen(credentials), 0);

            // Receive login response from server
            memset(buffer, 0, sizeof(buffer));
            read(sock, buffer, MAXLINE);
            printf("Server: %s\n", buffer);

            if (strcmp(buffer, "Login successful!") == 0) {
                int action_exit_flag = 0;

                // Infinite loop for role-based actions
                printf("You are logged in as a %s.\n", role);
                while (!action_exit_flag) {
                    
                    printf("Enter 'exit' to logout or choose an action: ");
                    char action[1024];
                    //scanf("%s", action);
                    fgets(action, sizeof(action), stdin);

                    // Send the action to the server
                    send(sock, action, strlen(action), 0);

                    // Receive response from server
                    memset(buffer, 0, sizeof(buffer));
                    read(sock, buffer, MAXLINE);
                    printf("Server: %s\n", buffer);

                    if (strcmp(action, "exit") == 0) {
                        action_exit_flag = 1;
                    }
                    // }else{
                    //     if (strcmp(role, "Admin")==0){
                    //         handle_admin_login(sock);
                    //     }
                    // }
                }
            } else {
                printf("Login failed. Try again.\n");
            }
        }
    }

    //close(sock);
    return 0;
}