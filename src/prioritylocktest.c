#include "prioritylock.h"
#include "types.h"
#include "user.h"

#define NCHILD 10

struct prioritylock testlk;


int main() {
    int pid;
    initprioritylock(&testlk, "test lock");
    pid = getpid();

    for (int i = 0; i < NCHILD; i++) {
        if (pid > 0)
            pid = fork(); 
    }  
    if (pid < 0)
        printf(2, "fork error\n");

    if (pid == 0) {
        acquirepriority(&testlk);
        // critical section
        printf(1, "%d acuired lock.\n", getpid());
        printpriorityqueue(&testlk);
        for (int i = 0; i < 1000000; i++);
        releasepriority(&testlk);
        printf(1, "%d released lock.\n", getpid());

    } else {
        for (int i = 0; i < NCHILD; i++) {
            wait();
        }

        printf(1, "End of test program");
    }


    return 0;
}