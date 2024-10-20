#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <time.h>
#include <sys/socket.h>
#include "bankDatabase.h"  
#include "common.h"        
#include "lock_util.h"
#include "utils.h" 

#define CUSTOMER_FILE "customers.dat"
#define LOAN_FILE "loans.dat"
#define TRANSACTION_FILE "transaction.dat"
#define EMPLOYEE_FILE "employees.dat"

void add_new_customer(int client_socket) {
    struct Customer customer;
    char buffer[1024];

    // Get customer details from the client
    memset(buffer, 0, sizeof(buffer));
    strcpy(buffer, "Enter Customer ID: ");
    send(client_socket, buffer, strlen(buffer), 0);
    memset(buffer, 0, sizeof(buffer));
    recv(client_socket, buffer, sizeof(buffer), 0);
    customer.id = atoi(buffer);

    memset(buffer, 0, sizeof(buffer));
    strcpy(buffer, "Enter Name: ");
    send(client_socket, buffer, strlen(buffer), 0);
    memset(buffer, 0, sizeof(buffer));
    recv(client_socket, buffer, sizeof(buffer), 0);
    strcpy(customer.name, buffer);

    memset(buffer, 0, sizeof(buffer));
    strcpy(buffer, "Enter Password: ");
    send(client_socket, buffer, strlen(buffer), 0);
    memset(buffer, 0, sizeof(buffer));
    recv(client_socket, buffer, sizeof(buffer), 0);
    printf("Before Hashing password..%s\n", buffer);
    unsigned long hashed_input_password = hash_password(buffer);
    printf("Hashed Password = %lu\n", hashed_input_password);

    customer.password = hashed_input_password;
    customer.balance = 0;
    customer.isActive = 1;

    generate_account_number(customer.accountNum, customer.id);
    printf("Generated Account Number: %s\n", customer.accountNum);   

    memset(buffer, 0, sizeof(buffer));
    int fd = open(CUSTOMER_FILE, O_WRONLY | O_APPEND);
    if (fd == -1) {
        perror("Error opening customer file");
        strcpy(buffer, "Error adding new customer\n");
        send(client_socket, buffer, strlen(buffer), 0);
        return;
    }

    lock_file(fd, F_WRLCK);
    write(fd, &customer, sizeof(customer));
    lock_file(fd, F_UNLCK);
    close(fd);

    strcpy(buffer, "New customer added successfully!\n");
    send(client_socket, buffer, strlen(buffer), 0);
}

void modify_customer_details(int client_socket) {
    int id;
    struct Customer customer;
    char buffer[1024];
    memset(buffer, 0, sizeof(buffer));
    strcpy(buffer, "Enter Customer ID to modify: ");
    send(client_socket, buffer, strlen(buffer), 0);
    recv(client_socket, buffer, sizeof(buffer), 0);
    id = atoi(buffer);

    int fd = open(CUSTOMER_FILE, O_RDWR);
    if (fd == -1) {
        perror("Error opening customer file");
        strcpy(buffer, "Error opening customer file\n");
        send(client_socket, buffer, strlen(buffer), 0);
        return;
    }

    lock_file(fd, F_WRLCK);
    while (read(fd, &customer, sizeof(customer)) > 0) {
        if (customer.id == id) {
            memset(buffer, 0, sizeof(buffer));
            strcpy(buffer, "Enter New Name: ");
            send(client_socket, buffer, strlen(buffer), 0);
            recv(client_socket, buffer, sizeof(buffer), 0);
            customer.id = atoi(buffer);

            memset(buffer, 0, sizeof(buffer));
            strcpy(buffer, "Enter New Password: ");
            send(client_socket, buffer, strlen(buffer), 0);
            //recv(client_socket, customer.password, sizeof(customer.password), 0);
            recv(client_socket, buffer, sizeof(buffer), 0);
            unsigned long hashed_input_password = hash_password(buffer);

            customer.password = hashed_input_password; 

            lseek(fd, -sizeof(customer), SEEK_CUR);
            write(fd, &customer, sizeof(customer));
            lock_file(fd, F_UNLCK);
            close(fd);

            strcpy(buffer, "Customer details updated successfully!\n");
            send(client_socket, buffer, strlen(buffer), 0);
            return;
        }
    }
    lock_file(fd, F_UNLCK);
    close(fd);

    strcpy(buffer, "Customer not found\n");
    send(client_socket, buffer, strlen(buffer), 0);
}



void view_customer_transactions(int client_socket) {
    int id;
    char buffer[1024];

    // Request customer ID
    strcpy(buffer, "Enter Customer ID to view transactions: ");
    send(client_socket, buffer, strlen(buffer), 0);
    
    // Receive customer ID from client
    recv(client_socket, buffer, sizeof(buffer), 0);
    id = atoi(buffer);

    // // Open transactions file
    // FILE *file = fopen(TRANSACTION_FILE, "rb"); // Use "rb" for binary read
    // if (!file) {
    //     perror("Error opening transactions file");
    //     strcpy(buffer, "Error opening transactions file\n");
    //     send(client_socket, buffer, strlen(buffer), 0);
    //     return;
    // }

    // // Notify about transactions start
    // strcpy(buffer, "Transactions:\n");
    // send(client_socket, buffer, strlen(buffer), 0);

    // struct Transaction txn;
    // int found = 0; // To check if any transactions are found
    // while (fread(&txn, sizeof(txn), 1, file) == 1) { // Check for successful read
    //     if (txn.customerId == id) {
    //         sprintf(buffer, "Date: %s, Amount: %.2f\n", txn.date, txn.amount);
    //         send(client_socket, buffer, strlen(buffer), 0);
    //         found = 1; // Set found to true if a transaction is found
    //     }
    // }

    // if (!found) {
    //     strcpy(buffer, "No transactions found for this customer ID.\n");
    //     send(client_socket, buffer, strlen(buffer), 0);
    // }

    // fclose(file);
    int fd = open(TRANSACTION_FILE, O_RDONLY);
    struct Transaction txn;
    int found = 0;
    lock_file(fd, F_WRLCK);
    while(read(fd, &txn, sizeof(txn))>0){
        if (txn.customerId==id){
            sprintf(buffer, "\nDescription: %s\t Date: %s\t Amount: %.2f\n", txn.description, txn.date, txn.amount);
            send(client_socket, buffer, strlen(buffer), 0);
            found = 1;
        }
    }

    if (found==0){
        strcpy(buffer, "No Records Found\n");
        send(client_socket, buffer, strlen(buffer), 0);
    }
    lock_file(fd, F_UNLCK);
    close(fd);
}


void change_employee_password(int client_socket, int emp_id) {
    char buffer[1024], new_password[50];
    struct Customer employee;

    strcpy(buffer, "Enter New Password: ");
    send(client_socket, buffer, strlen(buffer), 0);
    recv(client_socket, new_password, sizeof(new_password), 0);

    int fd = open(EMPLOYEE_FILE, O_RDWR);
    if (fd == -1) {
        perror("Error opening employee file");
        strcpy(buffer, "Error opening employee file\n");
        send(client_socket, buffer, strlen(buffer), 0);
        return;
    }

    lock_file(fd, F_WRLCK);
    while (read(fd, &employee, sizeof(employee)) > 0) {
        if (employee.id == emp_id) {
            //strcpy(employee.password, new_password);
            //recv(client_socket, buffer, sizeof(buffer), 0);
            unsigned long hashed_input_password = hash_password(new_password);
            employee.password = hashed_input_password; 
            lseek(fd, -sizeof(employee), SEEK_CUR);
            write(fd, &employee, sizeof(employee));
            lock_file(fd, F_UNLCK);
            close(fd);

            strcpy(buffer, "Password changed successfully!\n");
            send(client_socket, buffer, strlen(buffer), 0);
            return;
        }
    }
    lock_file(fd, F_UNLCK);
    close(fd);

    strcpy(buffer, "Employee not found\n");
    send(client_socket, buffer, strlen(buffer), 0);
}

void approve_reject_loan(int client_socket, int emp_id) {
    int loan_id;
    struct Loan loan;
    char buffer[1024];

    strcpy(buffer, "Enter Loan ID to approve/reject: ");
    send(client_socket, buffer, strlen(buffer), 0);
    recv(client_socket, buffer, sizeof(buffer), 0);
    loan_id = atoi(buffer);
    printf("Loan Id from client= %d\n", loan_id);

    int fd = open(LOAN_FILE, O_RDWR);
    if (fd == -1) {
        perror("Error opening loan file");
        strcpy(buffer, "Error opening loan file\n");
        send(client_socket, buffer, strlen(buffer), 0);
        return;
    }

    lock_file(fd, F_WRLCK);
    while (read(fd, &loan, sizeof(loan)) > 0) {
        printf("Loan ID...%d\n", loan.loanId);
        printf("Assigned to..%d\n", loan.assigned_to);
        printf("Emp Id= %d\n", emp_id);
        //printf(loan.loanId == loan_id);
        //printf(loan.assigned_to == emp_id);
        //printf(loan.loanId == loan_id && loan.assigned_to == emp_id);
        if (loan.loanId == loan_id && loan.assigned_to == emp_id) {
            strcpy(buffer, "Enter 'A' to Approve or 'R' to Reject: ");
            send(client_socket, buffer, strlen(buffer), 0);
            recv(client_socket, buffer, sizeof(buffer), 0);

            if (buffer[0] == 'A') {
                strcpy(loan.status, "Approved");
                loan.approved = 1;
            } else {
                strcpy(loan.status, "Rejected");
                loan.approved = -1;
            }

            lseek(fd, -sizeof(loan), SEEK_CUR);
            write(fd, &loan, sizeof(loan));
            lock_file(fd, F_UNLCK);
            close(fd);

            strcpy(buffer, "Loan status updated successfully!\n");
            send(client_socket, buffer, strlen(buffer), 0);
            return;
        }
    }
    lock_file(fd, F_UNLCK);
    close(fd);

    strcpy(buffer, "Loan not found or not assigned to you\n");
    send(client_socket, buffer, strlen(buffer), 0);
}


void view_assigned_loan_applications(int client_socket, int emp_id) {
    struct Loan loan;
    char buffer[5000];
    int found = 0;
    
    // printf("Hi Assigned...\n");
    // int fd = open(LOAN_FILE, O_RDONLY);
    // if (fd == -1) {
    //     perror("Error opening loan file");
    //     snprintf(buffer, sizeof(buffer), "Error opening loan file\n");
    //     send(client_socket, buffer, strlen(buffer), 0);
    //     return;
    // }
    
    // printf("File opened...\n");
    // snprintf(buffer, sizeof(buffer), "Assigned Loans:\n");
    // if (send(client_socket, buffer, strlen(buffer), 0) == -1) {
    //     perror("Error sending loan list header");
    //     close(fd);
    //     return;
    // }

    // printf("After file opened...\n");
    // while (read(fd, &loan, sizeof(loan)) > 0) {  
    //     printf("Inside while...");
    //     if (loan.assigned_to == emp_id) {
    //         // Use snprintf instead of strcpy for formatted string
    //         snprintf(buffer, sizeof(buffer),
    //                  "Loan ID: %d, Customer ID: %d, Amount: %.2f, Status: %d\n", 
    //                  loan.loanId, loan.customerId, loan.amount, loan.approved);
    //         send(client_socket, buffer, strlen(buffer), 0);
    //         found = 1;
    //     }
    // }

    // close(fd);

    // if (!found) {
    //     snprintf(buffer, sizeof(buffer), "No loans assigned to you.\n");
    //     send(client_socket, buffer, strlen(buffer), 0);
    // }

    int fd = open(LOAN_FILE, O_RDONLY);
    lock_file(fd, F_RDLCK);
    while (read(fd, &loan, sizeof(loan))>0){
        if (loan.assigned_to==emp_id){
            snprintf(buffer, sizeof(buffer), "Loan ID: %d\t Assigned_to:%d\t CustomerId: %d\t Amount: %f\t Approval Status:%d\n", loan.loanId, loan.assigned_to, loan.customerId, loan.amount, loan.approved);
            send(client_socket, buffer, strlen(buffer), 0);
            found = 1;
        }
    }

    if (found==0){
        strcpy(buffer, "No Records Found\n");
        send(client_socket, buffer, strlen(buffer), 0);
    }
    lock_file(fd, F_UNLCK);
    close(fd);
}



void generate_account_number(char *account_number, int customer_id) {
    time_t now = time(NULL);  // Get the current timestamp.
    snprintf(account_number, 50, "%d%ld", customer_id, now);  // Combine customer ID and timestamp.
}