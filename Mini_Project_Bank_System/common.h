// Common.h
#ifndef COMMON_H
#define COMMON_H

#pragma pack(1)
struct Customer {
    int id;
    char name[50];
    unsigned long password;
    float balance;
    int isActive;
    char accountNum[50];
};
#pragma pack()

#pragma pack(1)
struct Employee {
    int id;
    char name[50];
    unsigned long password;
    char role[50];
    int isActive;
};
#pragma pack()

#pragma pack(1)
struct Transaction {
    int customerId;
    char description[100];
    float amount;
    char date[11]; 
};
#pragma pack()

#pragma pack(1)
struct Loan {
    int loanId;
    int customerId;
    float amount;
    char status[20];
    int assigned_to;
    int approved; // 0 = pending, 1 = approved, -1 = rejected
};
#pragma pack()

#pragma pack(1)
struct Feedback {
    int customerId;
    char feedbackDesc[1024];
};
#pragma pack()
// Shared constants for filenames
#define CUSTOMER_FILE "customers.dat"
#define EMPLOYEE_FILE "employees.dat"

// Function declarations for shared utility functions
void init_files();            // Initializes data files if not present
void clear_screen();          // Cross-platform clear screen function
#endif
