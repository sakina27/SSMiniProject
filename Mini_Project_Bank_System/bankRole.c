#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <fcntl.h>
#include "bankDatabase.h"
#include "customer.h"
#include "admin.h"
#include "common.h"
#include "employee.h"
#include "managers.h"


#define CUSTOMER_FILE "customers.dat"
#define EMPLOYEE_FILE "employees.dat"

char* fetchCustomerAccNum(int empid);

// Example function for customer login
void customer_login(int client_socket, int empid) {
    char buffer[1024];
    int choice;
    int exit_flag = 0;

    char* account_number = fetchCustomerAccNum(empid);

    while (!exit_flag) {
        // Display menu options
        strcpy(buffer, 
            "===============Customer Portal===============\n"
            "1. View Account Balance\n"
            "2. Deposit Money\n"
            "3. Withdraw Money\n"
            "4. Transfer Funds\n"
            "5. Apply for a Loan\n"
            "6. Change Password\n"
            "7. Add Feedback\n"
            "8. View Transaction History\n"
            "9. Logout\n"
            "10. Exit\n"
            "Enter your choice: ");
        send(client_socket, buffer, strlen(buffer), 0);

        // Receive customer input
        recv(client_socket, buffer, sizeof(buffer), 0);
        choice = atoi(buffer);  // Convert input to integer

        switch (choice) {
            case 1: view_balance(client_socket, account_number); break;
            case 2: deposit_money(client_socket, account_number, empid); break;
            case 3: withdraw_money(client_socket, account_number, empid); break;
            case 4: transfer_funds(client_socket, account_number, empid); break;
            case 5: apply_for_loan(client_socket, account_number, empid); break;
            case 6: change_password_customer(client_socket, account_number, empid); break;
            case 7: add_feedback(client_socket, empid); break;
            case 8: view_transaction_history(client_socket, empid); break;
            case 9:
                strcpy(buffer, "Logging out...\n");
                send(client_socket, buffer, strlen(buffer), 0);
                //close(client_socket);  // Close socket
                return; // End session
            case 10:
                strcpy(buffer, "Exiting...\n");
                send(client_socket, buffer, strlen(buffer), 0);
                close(client_socket); 
                //exit(0);  // Terminate program
            default:
                strcpy(buffer, "Invalid choice. Please try again.\n");
                send(client_socket, buffer, strlen(buffer), 0);
        }
    }
}

// Employee, Manager, Administrator login functions
void employee_login(int client_socket, int empid) {
    // Similar to customer_login
    char buffer[1024];
    int choice;

    while (1) {
        // Display menu options
        strcpy(buffer, 
            "============Employee Portal=====================\n"
            "1. Add New Customer\n"
            "2. Modify Customer Details\n"
            "3. Approve/Reject Loans\n"
            "4. View Assigned Loan Applications\n"
            "5. View Customer Transactions\n"
            "6. Change Password\n"
            "7. Logout\n"
            "8. Exit\n"
            "Enter your choice: ");
        send(client_socket, buffer, strlen(buffer), 0);

        // Receive customer input
        recv(client_socket, buffer, sizeof(buffer), 0);
        choice = atoi(buffer);  // Convert input to integer

        switch (choice) {
            case 1:
                add_new_customer(client_socket);
                break;
            case 2:
                modify_customer_details(client_socket);
                break;
            case 3:
                approve_reject_loan(client_socket, empid);
                break;
            case 4:
                printf("Case 5...");
                view_assigned_loan_applications(client_socket, empid);
                break;
            case 5:
                view_customer_transactions(client_socket);
                break;
            case 6:
                change_employee_password(client_socket, atoi(empid));
                break;
            case 7:
                strcpy(buffer, "Logging out...\n");
                send(client_socket, buffer, strlen(buffer), 0);
                //close(client_socket);  // Close socket
                return;
            case 8:
                strcpy(buffer, "Exiting...\n");
                send(client_socket, buffer, strlen(buffer), 0);
                //exit(0);
                close(client_socket);
            default:
                strcpy(buffer, "Invalid choice. Please try again.\n");
                send(client_socket, buffer, strlen(buffer), 0);
        }
    }            
}

void manager_login(int client_socket, int managerID) {
    // Similar to customer_login
    char buffer[1024];
    int choice;

    while (1) {
        // Display menu options
        strcpy(buffer, 
            "============Manager Portal=====================\n"
            "1. Activate/Deactivate Customer Accounts\n"
            "2. Assign Loan Application Processes to Employees\n"
            "3. Review Customer Feedback\n"
            "4. Change Password\n"
            "5. Logout\n"
            "6. Exit\n"
            "Enter your choice: ");
        send(client_socket, buffer, strlen(buffer), 0);
        recv(client_socket, buffer, sizeof(buffer), 0);
        choice = atoi(buffer);

        switch (choice) {
            case 1:
                activate_deactivate_account(client_socket);
                break;
            case 2:
                assign_loan_application(client_socket);
                break;
            case 3:
                review_feedback(client_socket);
                break;
            case 4:
                change_manager_password(client_socket, managerID);
                break;
            case 5:
                strcpy(buffer, "Logging out...\n");
                send(client_socket, buffer, strlen(buffer), 0);
                //close(client_socket);  // Close socket
                return; 
            case 6:
                strcpy(buffer, "Exiting...\n");
                send(client_socket, buffer, strlen(buffer), 0);
                exit(0);  // Terminate program
            default:
                strcpy(buffer, "Invalid choice. Please try again.\n");
                send(client_socket, buffer, strlen(buffer), 0);
        }
}
}

void admin_login(int client_socket) {
        char buffer[1024];
        int choice;

        while (1) {
            memset(buffer, 0, sizeof(buffer));
            strcpy(buffer,"\n=========== Admin Menu ==============\n"
                                  "1. Add New Employee\n"
                                  "2. Modify Customer Details\n"
                                  "3. Modify Employee Details\n"
                                  "4. Assign Role to Employee\n"
                                  "5. Change User Password\n"
                                  "6. Logout\n"
                                  "7. Exit\n"
                                  "Enter your choice: ");
            send(client_socket, buffer, strlen(buffer), 0);
            recv(client_socket, buffer, sizeof(buffer), 0);
            choice = atoi(buffer); 
            switch (choice) {
                case 1:
                    add_new_employee(client_socket);
                    break;
                case 2:
                    modify_user_details(client_socket, CUSTOMER_FILE);
                    break;
                case 3:
                    modify_user_details(client_socket,EMPLOYEE_FILE);
                    break;
                case 4:
                    assign_role(client_socket);
                    break;
                case 5:
                    memset(buffer, 0, sizeof(buffer));
                    strcpy(buffer,"Change Password for:\n1. Customer\n2. Employee\nChoice: ");
                    send(client_socket, buffer, strlen(buffer), 0);
                    recv(client_socket, buffer, sizeof(buffer), 0);
                    int user_choice = atoi(buffer);
                    if (user_choice == 1)
                        change_user_password(client_socket, CUSTOMER_FILE);
                    else if (user_choice == 2)
                        change_user_password(client_socket, EMPLOYEE_FILE);
                    else
                        printf("Invalid choice!\n");
                    break;
            case 6:
                memset(buffer, 0, sizeof(buffer));
                strcpy(buffer, "Logging out...\n");
                send(client_socket, buffer, strlen(buffer), 0);
                //close(client_socket);  // Close socket
                return; 
            case 7:
                memset(buffer, 0, sizeof(buffer));
                strcpy(buffer, "Exiting...\n");
                send(client_socket, buffer, strlen(buffer), 0);
                close(client_socket);  // Terminate program
            default:
                memset(buffer, 0, sizeof(buffer));
                strcpy(buffer, "Invalid choice. Please try again.\n");
                send(client_socket, buffer, strlen(buffer), 0);
        }
        }
    }


char* fetchCustomerAccNum(int empid) {
    struct Customer customer;

    // Allocate memory for account number.
    char* account_number = (char*)malloc(50 * sizeof(char));
    if (!account_number) {
        perror("Memory allocation failed");
        return NULL;
    }

    // Open the customer file.
    int fd = open(CUSTOMER_FILE, O_RDONLY);
    if (fd == -1) {
        perror("Error opening customer file");
        free(account_number);  // Free allocated memory before returning.
        return NULL;
    }

    // Search for the customer with the given ID.
    while (read(fd, &customer, sizeof(customer)) > 0) {
        if (customer.id == empid) {
            strcpy(account_number, customer.accountNum);  // Copy the account number.
            close(fd);  // Close the file.
            return account_number;
        }
    }

    // If no matching customer is found.
    close(fd);
    free(account_number);  // Free memory if not used.
    return NULL;
}
