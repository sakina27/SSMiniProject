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
#include "customer.h"

#define CUSTOMER_FILE "customers.dat"
#define LOAN_FILE "loans.dat"
#define TRANSACTION_FILE "transaction.dat"
#define FEEDBACK_FILE "feedback.dat"
#define FEEDBACK_LEN 1024


void view_balance(int client_socket, const char *account_number) {
    char buffer[1024];
    printf("Account Number=%s\n", account_number);
    double balance = get_balance(account_number);  // Fetch balance from DB
    snprintf(buffer, sizeof(buffer), "Your current balance is: $%.2f\n", balance);
    send(client_socket, buffer, strlen(buffer), 0);
}

void deposit_money(int client_socket, const char *account_number, int empid) {
    char buffer[1024];
    double amount;
    memset(buffer, 0, sizeof(buffer));
    strcpy(buffer, "Enter amount to deposit: ");
    send(client_socket, buffer, strlen(buffer), 0);
    recv(client_socket, buffer, sizeof(buffer), 0);
    amount = atof(buffer);

    if (amount > 0) {
        update_balance(account_number, amount);  // Update balance in DB
        strcpy(buffer, "Deposit successful!\n");
        char operation[50];
        strcpy(operation,"Credit");
        update_transaction_history(operation, empid, amount);
    } else {
        strcpy(buffer, "Invalid amount. Please try again.\n");
    }
    send(client_socket, buffer, strlen(buffer), 0);
    

    
}

void withdraw_money(int client_socket, const char *account_number, int empid) {
    char buffer[1024];
    double amount, current_balance = get_balance(account_number);

    memset(buffer, 0, sizeof(buffer));
    strcpy(buffer, "Enter amount to withdraw: ");
    send(client_socket, buffer, strlen(buffer), 0);
    memset(buffer, 0, sizeof(buffer));
    recv(client_socket, buffer, sizeof(buffer), 0);
    amount = atof(buffer);

    if (amount > 0 && amount <= current_balance) {
        update_balance(account_number, -amount);  // Deduct amount
        strcpy(buffer, "Withdrawal successful!\n");
        char operation[50];
        strcpy(operation,"Debit");
        update_transaction_history(operation, empid, amount);
    } else {
        strcpy(buffer, amount > current_balance ? 
            "Insufficient balance.\n" : "Invalid amount. Please try again.\n");
    }
    send(client_socket, buffer, strlen(buffer), 0);
}

void transfer_funds(int client_socket, const char *account_number, int empid) {
    char buffer[1024], target_account[50];
    double amount;
    memset(buffer, 0, sizeof(buffer));
    strcpy(buffer, "Enter target account number: ");
    send(client_socket, buffer, strlen(buffer), 0);
    memset(buffer, 0, sizeof(buffer));
    //recv(client_socket, target_account, sizeof(target_account), 0);
    recv(client_socket, buffer, sizeof(buffer), 0);
    strcpy(target_account, buffer);

    printf("Target Account Number= %s\n",target_account);
    memset(buffer, 0, sizeof(buffer));
    strcpy(buffer, "Enter amount to transfer: ");
    send(client_socket, buffer, strlen(buffer), 0);

    memset(buffer, 0, sizeof(buffer));
    recv(client_socket, buffer, sizeof(buffer), 0);
    amount = atof(buffer);

    if (amount > 0 && get_balance(account_number) >= amount) {
        update_balance(account_number, -amount);  // Deduct from sender
        printf("Target Account Number= %s\n",target_account);
        update_balance(target_account, amount);  // Add to receiver
        strcpy(buffer, "Transfer successful!\n");
        char operation[50];
        strcpy(operation,"Transfer");
        update_transaction_history(operation, empid, amount);
    } else {
        strcpy(buffer, "Transfer failed. Check balance or input.\n");
    }
    send(client_socket, buffer, strlen(buffer), 0);
}

void apply_for_loan(int client_socket, const char *account_number, int empid) {
    char buffer[1024];
    double amount;
    struct Loan loan;

    memset(buffer, 0, sizeof(buffer));
    strcpy(buffer, "Enter loan amount: ");
    send(client_socket, buffer, strlen(buffer), 0);
    recv(client_socket, buffer, sizeof(buffer), 0);
    amount = atof(buffer);
    
    char loan_id[50];
    generate_loan_id(loan_id, empid);
    printf("Loan Id = %s\n", loan_id);
    loan.loanId = atoi(loan_id);
    loan.customerId = empid;
    loan.amount = amount;
    loan.approved = 0;
    loan.assigned_to = 0;


    // Simulate loan approval logic (simplified)
    if (amount > 0) {
        strcpy(buffer, "Loan application submitted. We will notify you shortly.\n");
        int fd = open(LOAN_FILE, O_RDWR | O_APPEND);
        lock_file(fd, F_WRLCK);
        write(fd, &loan, sizeof(loan));
        lock_file(fd, F_UNLCK);
        close(fd);
    } else {
        strcpy(buffer, "Invalid amount. Please try again.\n");
    }
    send(client_socket, buffer, strlen(buffer), 0);
}

void change_password_customer(int client_socket, const char *account_number, int empid) {
    char buffer[1024], new_password[50];


    memset(buffer, 0, sizeof(buffer));
    strcpy(buffer, "Enter new password: ");
    send(client_socket, buffer, strlen(buffer), 0);
    recv(client_socket, new_password, sizeof(new_password), 0);

    update_password(client_socket,account_number, new_password, empid);  // Update password in DB
    strcpy(buffer, "Password changed successfully!\n");
    send(client_socket, buffer, strlen(buffer), 0);
}

// void add_feedback(int client_socket, int empid) {
//     char buffer[1024];
//     char feedback[1024];

//     // Prompt user for feedback
//     strcpy(buffer, "Enter your feedback: ");
//     send(client_socket, buffer, strlen(buffer), 0);
//     memset(buffer, 0, sizeof(buffer));

//     // Receive feedback from the client
//     int bytes_read = recv(client_socket, buffer, sizeof(buffer) - 1, 0);
//     if (bytes_read < 0) {
//         perror("Error reading feedback");
//         return;
//     }

//     buffer[bytes_read] = '\0';  // Null-terminate the received data
//     printf("Feedback received: %s\n", buffer);

//     strcpy(feedback, buffer);  // Save the feedback
//     save_feedback(feedback, empid);

//     // Send acknowledgment
//     strcpy(buffer, "Thank you for your feedback!\n");
//     send(client_socket, buffer, strlen(buffer), 0);
// }


void add_feedback(int client_socket, int empid) {
    char buffer[1024];
    char feedback[1024];

    // Prompt user for feedback
    strcpy(buffer, "Enter your feedback: ");
    send(client_socket, buffer, strlen(buffer), 0);
    memset(buffer, 0, sizeof(buffer));

    // Receive feedback from the client
    int bytes_read = recv(client_socket, buffer, sizeof(buffer) - 1, 0);
    if (bytes_read < 0) {
        perror("Error reading feedback");
        return;
    }

    buffer[bytes_read] = '\0';  // Null-terminate the received data
    printf("Feedback received: %s\n", buffer);

    strcpy(feedback, buffer);  // Save the feedback
    save_feedback(feedback, empid);

    // Send acknowledgment
    strcpy(buffer, "Thank you for your feedback!\n");
    send(client_socket, buffer, strlen(buffer), 0);
}

void save_feedback(const char *feedback, int empid) {
    int fd = open(FEEDBACK_FILE, O_RDWR | O_APPEND);
    if (fd < 0) {
        perror("Error opening feedback file");
        return;
    }
    lock_file(fd, F_WRLCK);
    FILE *file = fopen(FEEDBACK_FILE, "a");
    if (file == NULL) {
        perror("Error opening feedback file");
        lock_file(fd, F_UNLCK);
        return;
    }

    // Save the feedback in a readable format
    fprintf(file, "Employee ID: %d\nFeedback: %s\n\n", empid, feedback);
    lock_file(fd, F_UNLCK);
    fclose(file);
}

void view_transaction_history(int client_socket, int empid) {
    char buffer[1024];
    int fd = open(TRANSACTION_FILE, O_RDONLY);
    struct Transaction txn;
    int found = 0;
    lock_file(fd, F_RDLCK);
    while(read(fd, &txn, sizeof(txn))>0){
        if (txn.customerId==empid){
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

double get_balance(const char *account_number) {
    int fd = open(CUSTOMER_FILE, O_RDONLY);
    double balance = 0;

    struct Customer customer;

    while(read(fd, &customer, sizeof(customer)) > 0){
        if (strcmp(customer.accountNum, account_number)==0){
            close(fd);
            return customer.balance;
        }
    }

    close(fd);
    return 0;
}

void update_balance(const char *account_number, double amount) {
     int fd = open(CUSTOMER_FILE, O_RDWR);
    double balance = 0;
    lock_file(fd, F_RDLCK);
    struct Customer customer;
    printf("Account Number=%s\n", account_number);
    while(read(fd, &customer, sizeof(customer)) > 0){
        if (strcmp(customer.accountNum, account_number)==0){
            balance = customer.balance + amount;
            customer.balance = balance;
            lseek(fd, -sizeof(customer), SEEK_CUR);
            write(fd, &customer, sizeof(customer));
            close(fd);
            return;
        }
    }
    lock_file(fd, F_UNLCK);
    close(fd);
    return;
}

void update_password(int client_socket, const char *account_number, const char *new_password, int empid) {
    char buffer[1024];
    struct Customer customer;  // Assuming Customer struct contains id, name, password, role, etc.

    // Prompt for new password
    // memset(buffer, 0, sizeof(buffer));
    // strcpy(buffer, "Enter New Password: ");
    // send(client_socket, buffer, strlen(buffer), 0);
    // recv(client_socket, buffer, sizeof(buffer), 0);
    // strcpy(new_password, buffer);
    

    // Open the employee database file
    int fd = open(CUSTOMER_FILE, O_RDWR);
    // if (fd == -1) {
    //     perror("Error opening employee file");
    //     strcpy(buffer, "Error opening employee file");
    //     send(client_socket, buffer, strlen(buffer), 0);
    //     return;
    // }

    lock_file(fd, F_WRLCK);
    int found = 0;
    while (read(fd, &customer, sizeof(customer)) > 0) {
          if (customer.id == empid){
              unsigned long hashed_input_password = hash_password(new_password);
              customer.password = hashed_input_password;
              lseek(fd, -sizeof(customer), SEEK_CUR);
              write(fd, &customer, sizeof(customer));

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
        strcpy(buffer, "Employee not found \n");
    }
    send(client_socket, buffer, strlen(buffer), 0);
   
}



// void save_feedback(const char *feedback, int empid) {
//     int fd = open(FEEDBACK_FILE, O_RDWR | O_APPEND);
//     struct Feedback feedbk;
//     char buffer[100];

//     feedbk.customerId = empid;
//     strcpy(feedbk.feedbackDesc, feedback);
//     write(fd, &feedbk, sizeof(feedbk));
//     close(fd);
// }


void generate_loan_id(char *loanId, int customer_id) {
    time_t now = time(NULL);  // Get the current timestamp.
    snprintf(loanId, 50, "%ld%d", now, customer_id);  // Combine customer ID and timestamp.
}

void update_transaction_history(char *operation, int empid, double amount){
     int fd = open(TRANSACTION_FILE, O_RDWR | O_APPEND);

     struct Transaction txn;

     txn.customerId = empid;
     strcpy(txn.description, operation);
     txn.amount = amount;

    time_t t = time(NULL);
    
    // Check if time() returns a valid time
    if (t == ((time_t)-1)) {
        perror("Failed to obtain current time");
        return 1;
    }

    // Convert time_t to struct tm
    struct tm *tm_info = localtime(&t);
    
    // Check if localtime() was successful
    if (tm_info == NULL) {
        perror("Failed to convert time to local time");
        return 1;
    }

    // Character array to hold the formatted date
    char date_string[11];  // Format "YYYY-MM-DD" requires 11 characters (including null terminator)

    // Format the date as "YYYY-MM-DD"
    strftime(txn.date, sizeof(txn.date), "%Y-%m-%d", tm_info);

    lock_file(fd, F_WRLCK);
    write(fd, &txn, sizeof(txn));
    lock_file(fd, F_UNLCK);
    close(fd);
}