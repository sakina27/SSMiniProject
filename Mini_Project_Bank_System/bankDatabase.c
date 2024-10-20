#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include "utils.h"
#include "common.h"
#include "lock_util.h"

#define CUSTOMER_FILE "customers.dat"
#define EMPLOYEE_FILE "employees.dat"
#define ADMIN_FILE "users.txt"

// This function simulates a database lookup and returns the role if the credentials match
int authenticate_user(const char *username, const char *password, char *role) {
    printf("Inside authenticate_user...\n");
    printf("Username Received: %s\n", username);
    printf("Password Received: %s\n", password);
    printf("Role Received: %s\n", role);
    role[strcspn(role, "\n")] = '\0';
    struct Employee employee;
    struct Customer customer;
    char fileName[10];
    int fd;
    

    printf("Comparing..%d\n", strcmp(role, "Customer"));
    if (strcmp(role, "Customer") == 0) {
        printf("Inside Customer..");
    fd = open(CUSTOMER_FILE, O_RDONLY);
    if (fd < 0) {
        perror("Error opening customer file");
        return 0;
    }else{
        printf("File Opened");
    }
    } else if (strcmp(role, "Employee") == 0 || strcmp(role, "Manager") == 0) {
        printf("Inside Employee/Manager..");
        fd = open(EMPLOYEE_FILE, O_RDONLY);
        printf("File Descriptor..%d\n", fd);
    if (fd < 0) {
        perror("Error opening employee file");
        return 0;
    }
    }else {
        printf("Unknown role: %s\n", role);
        return;
    }

    
    
    printf("getting1....%s\n", role);
    
    unsigned long hashed_input_password = hash_password(password);  // Hash the input password

    printf("Hashed Password..%lu\n", hashed_input_password);
    printf("Comparing..%d\n", strcmp(role, "Customer"));

    if (strcmp(role, "Customer")==0){
        printf("Comparing..%d\n", strcmp(role, "Customer"));
        printf("File Descriptor..%d\n",fd);
        role[strcspn(role, "\n")] = '\0';
        if (strcmp(role, "Customer")==0){

        printf("Inside if...");
        while (read(fd, &customer, sizeof(customer)) > 0) {
            printf("Customer Id-%d\t Customer Password-%lu\t ",customer.id, customer.password);
            if (customer.id==atoi(username) && customer.password==hashed_input_password){
                printf("Login Successful from authenticate");

                close(fd);
                return 1;
            }
        }
    }
    }
    if (strcmp(role, "Admin")==0){
        printf("Hi Admin...");
        FILE *file = fopen("users.txt", "r");
        if (file == NULL) {
        perror("Error opening users file");
        return 0;
        }
        char stored_username[50], stored_password[50], stored_role[50];
        while (fscanf(file, "%49s %49s %49s", stored_username, stored_password, stored_role) != EOF) {
            unsigned long stored_hashed_password = strtoul(stored_password, NULL, 10);  // Convert stored hash to unsigned long
            printf("getting2....%s\n", role);
        if (strcmp(stored_username, username) == 0 && stored_hashed_password == hashed_input_password && strcmp(stored_role, role)==0) {
            printf("getting3....%s\n", role);
            //strcpy(role, stored_role);
             printf("getting4....%s\n", role);
             fclose(file);
             return 1; // Successful login
        }
        }
        fclose(file);
    }else if (strcmp(role, "Employee")==0 || strcmp(role, "Manager")==0){
    printf("Hi not admin...");
    
        //lock_file(fd, F_WRLCK);
        printf("Comparing..%d\n", strcmp(role, "Customer"));
        while (read(fd, &employee, sizeof(employee)) > 0) {
            printf("Employee id-%d\nPassword-%lu\nRole-%s\n",employee.id,employee.password,employee.role);
            if (employee.id==atoi(username) && employee.password==hashed_input_password){
                printf("Login Successful from authenticate");
                //lock_file(fd, F_UNLCK);
                close(fd);
                return 1;
            }
        }   
    }
    
    
    return 0; // Login failed
}
