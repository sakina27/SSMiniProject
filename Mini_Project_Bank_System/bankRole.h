#ifndef BANKROLE_H
#define BANKROLE_H

// Include necessary libraries
#include <stdio.h>

// Function declarations for role-specific logins
void customer_login(int client_socket);
void employee_login(int client_socket);
void manager_login(int client_socket);
void admin_login(int client_socket);
char* fetchCustomerAccNum(int empid);

#endif // BANKROLE_H
