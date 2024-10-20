#include <stdio.h>
#include <string.h>

struct Employee {
    int id;
    char name[50];
    unsigned long password;
    char role[50];
    int isActive;
};

void write_sample_data(const char *filename) {
    FILE *file = fopen(filename, "wb");
    if (!file) {
        perror("Error opening file for writing");
        return;
    }

    struct Employee employees[] = {
        {1, "Alice Smith", 1234567890, "Admin", 1},
        {2, "Bob Johnson", 9876543210, "Employee", 1},
        {3, "Charlie Brown", 1112223334, "Customer", 0}
    };

    fwrite(employees, sizeof(employees), 1, file);
    fclose(file);
}

int main() {
    write_sample_data("employees.dat");
    return 0;
}
