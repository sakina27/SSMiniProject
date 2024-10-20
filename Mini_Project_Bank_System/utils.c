#include <stdio.h>

// Hash function for password (simplified)
unsigned long hash_password(const char *password) {
    unsigned long hash = 5381;
    int c;
    printf("Inside Hash...");
    while ((c = *password++))
        hash = ((hash << 5) + hash) + c;
    printf("After Hash..");
    return hash;
}