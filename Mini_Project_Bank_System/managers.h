#ifndef MANAGER_H
#define MANAGER_H

#include <sys/socket.h>  // For socket-related functions

// Function to activate or deactivate a customer account.
void activate_deactivate_account(int client_socket);

// Function to assign a loan application to an employee.
void assign_loan_application(int client_socket);

// Function to review customer feedback.
void review_feedback(int client_socket);

// Function to change the password of a manager.
void change_manager_password(int client_socket, int manager_id);

#endif  // MANAGER_H
