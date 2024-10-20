#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include "common.h"

// Initializes the customer and employee files if they don't exist
void init_files() {
    int fd;

    fd = open(CUSTOMER_FILE, O_CREAT | O_EXCL | O_WRONLY, 0644);
    if (fd != -1) {
        printf("Initializing customer data file...\n");
        close(fd);
    }

    fd = open(EMPLOYEE_FILE, O_CREAT | O_EXCL | O_WRONLY, 0644);
    if (fd != -1) {
        printf("Initializing employee data file...\n");
        close(fd);
    }
}

// Cross-platform clear screen function
void clear_screen() {
#ifdef _WIN32
    system("cls");
#else
    system("clear");
#endif
}