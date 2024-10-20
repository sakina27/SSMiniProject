#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct Employee {
    int id;
    char name[50];
    unsigned long password;  // Note: Store hashed password for security
    char role[50];
    int isActive;
};

void read_employee_records(const char *filename) {
    FILE *file = fopen(filename, "rb");
    if (!file) {
        perror("Error opening file");
        return;
    }

    struct Employee employee;
    printf("Employee Records:\n");
    printf("ID\tName\t\t\tPassword\tRole\t\tActive\n");
    printf("-----------------------------------------------------------\n");

    while (fread(&employee, sizeof(employee), 1, file) == 1) {
        printf("%d\t%-20s\t%lu\t\t%-10s\t%d\n",
               employee.id,
               employee.name,
               employee.password,
               employee.role,
               employee.isActive);
    }

    fclose(file);
}

int main() {
    read_employee_records("employees.dat");
    return 0;
}
