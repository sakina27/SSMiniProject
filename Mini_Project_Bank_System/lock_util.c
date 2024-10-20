#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>

void lock_file(int fd, short lock_type) {
    struct flock fl;
    fl.l_type = lock_type;
    fl.l_whence = SEEK_SET;
    fl.l_start = 0;
    fl.l_len = 0;  // Lock entire file

    if (fcntl(fd, F_SETLKW, &fl) == -1) {
        perror("Error locking file");
        _exit(1);
    }
}