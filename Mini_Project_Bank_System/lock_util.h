// lock_util.h
#ifndef LOCK_UTIL_H
#define LOCK_UTIL_H

// Lock types: F_WRLCK = Write lock, F_RDLCK = Read lock, F_UNLCK = Unlock
void lock_file(int fd, short lock_type);


#endif
