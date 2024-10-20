#ifndef BANK_OPERATIONS_H
#define BANK_OPERATIONS_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

// Function declarations
void view_balance(int client_socket, const char *account_number);
void deposit_money(int client_socket, const char *account_number, int empid);
void withdraw_money(int client_socket, const char *account_number, int empid);
void transfer_funds(int client_socket, const char *account_number, int empid);
void apply_for_loan(int client_socket, const char *account_number, int empid);
void change_password_customer(int client_socket, const char *account_number, int empid);
void add_feedback(int client_socket, int empid);
void view_transaction_history(int client_socket, int empid);

// Helper function declarations (assumed to be defined elsewhere)
double get_balance(const char *account_number);
void update_balance(const char *account_number, double amount);
void update_password(int client_socket, const char *account_number, const char *new_password, int empid);
void save_feedback(const char *feedback, int empid);

#endif  // BANK_OPERATIONS_H
