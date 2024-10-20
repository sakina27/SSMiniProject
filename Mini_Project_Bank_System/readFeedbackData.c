#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include "common.h"

#define FEEDBACK_FILE "feedback.dat"

int main() {
    int fd = open(FEEDBACK_FILE, O_RDONLY);
    struct Feedback feedback;

    while (read(fd, &feedback, sizeof(feedback)) > 0) {
        printf("Customer ID: %d\t Feedback: %s\n", feedback.customerId, feedback.feedbackDesc);
    }
    return 0;
}