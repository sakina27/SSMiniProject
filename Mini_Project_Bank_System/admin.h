#ifndef ADMIN_H
#define ADMIN_H

// Function declarations for admin operations
//void admin_menu();
void add_new_employee(int client_socket);
void modify_user_details(int client_socket, const char *filename);
void change_user_password(int client_socket, const char *filename);
void assign_role(int client_socket);

#endif // ADMIN_H
