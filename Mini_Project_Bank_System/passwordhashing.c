#include <stdio.h>
#include "utils.h"  // Ensure this has hash_password function

int main() {
    char password[50];
    
    printf("Enter password to hash: ");
    scanf("%s", password);
    
    unsigned long hashed_password = hash_password(password);
    printf("Hashed Password: %lu\n", hashed_password);
    
    return 0;
}
