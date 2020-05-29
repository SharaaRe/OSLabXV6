#include "types.h"
#include "stat.h"
#include "user.h"
#include "fcntl.h"
#include "fs.h"

int main(int argc, char **argv) {
    // open("test_file_1", O_CREATE);
    int n_syscalls = count_syscalls();
    printf(1, "%d\n", n_syscalls);

    exit();
}