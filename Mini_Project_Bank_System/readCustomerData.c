#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include "common.h"



int main() {
    int fd = open(CUSTOMER_FILE, O_RDONLY);
    struct Customer customer;

    while (read(fd, &customer, sizeof(customer)) > 0) {
        printf("Customer Id: %d\t Name: %s\t Password: %lu\t Balance: %f\t Account Number: %s\t isActive: %d\n", customer.id, customer.name, customer.password, customer.balance,customer.accountNum, customer.isActive);
    }
    return 0;
}