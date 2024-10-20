#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include "common.h"

#define LOAN_FILE "loans.dat"

int main() {
    int fd = open(LOAN_FILE, O_RDONLY);
    struct Loan loan;

    while (read(fd, &loan, sizeof(loan)) > 0) {
        printf("Loan Id: %d\t Customer Id: %d\t Amount: %f\t Assigned_to: %d\t Approval: %d\t Status:%s\n", loan.loanId, loan.customerId, loan.amount, loan.assigned_to, loan.approved, loan.status);
    }
    return 0;
}