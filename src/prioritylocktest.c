
#include "types.h"
#include "stat.h"
#include "user.h"
#include "fcntl.h"
#include "fs.h"

#define NCHILD 10

int main() {
    int pid;
    initprioritylocktest();
    pid = getpid();

    for (int i = 0; i < NCHILD; i++) {
        
        if (pid > 0) {
            pid = fork(); 
            // printf(1, "pid: %d forked\n", pid);
        }
    }  
    if (pid < 0)
        printf(2, "fork error\n");

    if (pid == 0) {
        prioritylocktest();

    } else {
        for (int i = 0; i < NCHILD; i++) {
            wait();
        }

        printf(1, "End of test program");
    }


    return 0;
}