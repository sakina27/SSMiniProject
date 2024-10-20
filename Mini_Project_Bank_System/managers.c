#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/socket.h>
#include "bankDatabase.h"
#include "common.h"
#include "lock_util.h"

#define CUSTOMER_FILE "customers.dat"
#define EMPLOYEE_FILE "employees.dat"
#define LOAN_FILE "loans.dat"
#define FEEDBACK_FILE "feedback.dat"

void activate_deactivate_account(int client_socket) {
    int id, status;
    struct Customer customer;
    char buffer[1024];

    strcpy(buffer, "Enter Customer ID to modify: ");
    send(client_socket, buffer, strlen(buffer), 0);
    recv(client_socket, buffer, sizeof(buffer), 0);
    id = atoi(buffer);

    int fd = open(CUSTOMER_FILE, O_RDWR);
    if (fd == -1) {
        perror("Error opening customer file");
        strcpy(buffer, "Error opening customer file");
        send(client_socket, buffer, strlen(buffer), 0);
        return;
    }

    lock_file(fd, F_WRLCK);
    while (read(fd, &customer, sizeof(customer)) > 0) {
        if (customer.id == id) {
            strcpy(buffer, "Enter 1 to Activate, 0 to Deactivate: ");
            send(client_socket, buffer, strlen(buffer), 0);
            recv(client_socket, buffer, sizeof(buffer), 0);
            status = atoi(buffer);

            customer.isActive = status;  // Modify active status
            lseek(fd, -sizeof(customer), SEEK_CUR);
            write(fd, &customer, sizeof(customer));

            strcpy(buffer, "Account status updated successfully.\n");
            send(client_socket, buffer, strlen(buffer), 0);
            lock_file(fd, F_UNLCK);
            close(fd);
            return;
        }
    }

    strcpy(buffer, "Customer not found.\n");
    send(client_socket, buffer, strlen(buffer), 0);
    lock_file(fd, F_UNLCK);
    close(fd);
}

void assign_loan_application(int client_socket) {
    int loan_id, emp_id;
    char buffer[1024];

    strcpy(buffer, "Enter Loan Application ID: ");
    send(client_socket, buffer, strlen(buffer), 0);
    recv(client_socket, buffer, sizeof(buffer), 0);
    loan_id = atoi(buffer);

    strcpy(buffer, "Enter Employee ID to assign: ");
    send(client_socket, buffer, strlen(buffer), 0);
    recv(client_socket, buffer, sizeof(buffer), 0);
    emp_id = atoi(buffer);

    int fd = open(LOAN_FILE, O_RDWR);
    if (fd == -1) {
        perror("Error opening loan file");
        strcpy(buffer, "Error opening loan file");
        send(client_socket, buffer, strlen(buffer), 0);
        return;
    }

    lock_file(fd, F_WRLCK);
    struct Loan loan;
    while (read(fd, &loan, sizeof(loan)) > 0) {
        if (loan.loanId == loan_id) {
            loan.assigned_to = emp_id;  // Assign employee to loan
            lseek(fd, -sizeof(loan), SEEK_CUR);
            write(fd, &loan, sizeof(loan));

            strcpy(buffer, "Loan application assigned successfully.\n");
            send(client_socket, buffer, strlen(buffer), 0);
            lock_file(fd, F_UNLCK);
            close(fd);
            return;
        }
    }

    strcpy(buffer, "Loan application not found.\n");
    send(client_socket, buffer, strlen(buffer), 0);
    lock_file(fd, F_UNLCK);
    close(fd);
}

void review_feedback(int client_socket) {
    int fd = open(FEEDBACK_FILE, O_RDONLY);
    char buffer[1024];

    if (fd == -1) {
        perror("Error opening feedback file");
        strcpy(buffer, "Error opening feedback file");
        send(client_socket, buffer, strlen(buffer), 0);
        return;
    }

    lock_file(fd, F_RDLCK);
    while (read(fd, buffer, sizeof(buffer)) > 0) {
        send(client_socket, buffer, strlen(buffer), 0);
    }

    lock_file(fd, F_UNLCK);
    close(fd);
}

void change_manager_password(int client_socket, int manager_id) {
    char buffer[1024], new_password[50];
    struct Employee employee;  // Assuming Customer struct contains id, name, password, role, etc.

    // Prompt for new password
    memset(buffer, 0, sizeof(buffer));
    strcpy(buffer, "Enter New Password: ");
    send(client_socket, buffer, strlen(buffer), 0);
    recv(client_socket, new_password, sizeof(new_password), 0);

    // Open the employee database file
    int fd = open(EMPLOYEE_FILE, O_RDWR);
    if (fd == -1) {
        perror("Error opening employee file");
        strcpy(buffer, "Error opening employee file");
        send(client_socket, buffer, strlen(buffer), 0);
        return;
    }

    lock_file(fd, F_WRLCK);  // Lock the file to avoid concurrent writes

    // Search for the manager's record
    int found = 0;
    while (read(fd, &employee, sizeof(employee)) > 0) {
        if (employee.id == manager_id && strcmp(employee.role, "Manager") == 0) {
            // Update the password
            //strcpy(employee.password, new_password);
            unsigned long hashed_input_password = hash_password(buffer);
            employee.password == hashed_input_password;

            // Move the file pointer back to overwrite the existing record
            lseek(fd, -sizeof(employee), SEEK_CUR);
            write(fd, &employee, sizeof(employee));

            found = 1;
            break;
        }
    }

    lock_file(fd, F_UNLCK);  // Unlock the file
    close(fd);

    // Send appropriate response to the client
    memset(buffer, 0, sizeof(buffer));
    if (found) {
        strcpy(buffer, "Password changed successfully.\n");
    } else {
        strcpy(buffer, "Manager not found or not authorized.\n");
    }
    send(client_socket, buffer, strlen(buffer), 0);
}