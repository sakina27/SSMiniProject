#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include "common.h"



int main() {
    int fd = open(EMPLOYEE_FILE, O_RDONLY);
    struct Employee employee;

    while (read(fd, &employee, sizeof(employee)) > 0) {
        printf("Employee Id: %d\t Name: %s\t Password: %lu\t Role: %s\t isActive: %d\n", employee.id, employee.name, employee.password,employee.role,employee.isActive);
    }
    return 0;
}