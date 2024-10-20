#ifndef BANK_DATABASE_H
#define BANK_DATABASE_H

// Include necessary headers
#include <stdio.h>
#include <string.h>

// Function prototype for authenticating a user
// Parameters:
// - username: the username to authenticate
// - password: the password to authenticate
// - role: the role to return if authentication is successful
// Returns 1 on successful authentication, 0 on failure
int authenticate_user(const char *username, const char *password, char *role);

#endif // BANK_DATABASE_H
