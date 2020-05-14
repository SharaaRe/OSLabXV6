#include "types.h"
#include "stat.h"
#include "user.h"
#include "fcntl.h"

int 
main (int argc, char **argv) {
    int k = 0, n = 0;
    float x = 0, z = 0, d = 0;

    if (argc < 2) {
        n = 1;
    } else {
        n = atoi(argv[1]);
    }

    if ( n < 0 || n > 20 ) {
        n = 2;
    }

    if (argc < 3) {
        d = 0.1;
    } else {
        d = atoi(argv[2]);
    }

    for (k = 0; k < n; ++k) {
        if (fork() == 0) {
            printf(1, "start child process %d\n", getpid());
            for (z = 0; z < (1<<24)*(k+1); z += d) {
                x = x + 3.14 * 13.13;
            }
            printf(1, "exit child process %d\n", getpid());
            exit();
        }
    }
    for (k = 0; k < n; ++k) {
        wait();
    }
    printf(1, "end\n");
    exit();
}