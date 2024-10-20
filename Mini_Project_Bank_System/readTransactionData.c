#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include "common.h"

#define TRANSACTION_FILE "transaction.dat"

int main() {
    int fd = open(TRANSACTION_FILE, O_RDONLY);
    struct Transaction txn;

    while (read(fd, &txn, sizeof(txn)) > 0) {
        printf("Customer Id: %d\t Description: %s\t Amount: %f\t Date: %s\n", txn.customerId, txn.description, txn.amount, txn.date);
    }
    return 0;
}