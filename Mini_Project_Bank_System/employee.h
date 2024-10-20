#ifndef EMPLOYEE_H
#define EMPLOYEE_H


// Function prototypes
void add_new_employee(int client_socket);  // Add a new employee
void modify_employee_details(int client_socket, int emp_id); // Modify employee details
void change_employee_password(int client_socket, int emp_id); // Change employee password
void view_assigned_loan_applications(int client_socket, int emp_id); // View assigned loans
void approve_reject_loan(int client_socket, int emp_id); // Approve or reject loan

#endif // EMPLOYEE_H
