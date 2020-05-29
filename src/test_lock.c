#include "types.h"
#include "stat.h"
#include "user.h"
#include "fcntl.h"
#include "fs.h"

int main(int argc, char **argv) {
    int fd1 = open("test_file_1", O_CREATE);
    int fd2 = open("test_file_2", O_CREATE);
    int fd3 = open("test_file_3", O_CREATE);
    int fd4 = open("test_file_4", O_CREATE);

    write(fd1, "salaam 1\n", 9);
    write(fd2, "salaam 2\n", 9);
    write(fd3, "salaam 3\n", 9);
    write(fd4, "salaam 4\n", 9);

    close(fd1);
    close(fd2);
    close(fd3);
    close(fd4);

    int n_syscalls = count_syscalls();
    printf(1, "%d\n", n_syscalls);

    exit();
}