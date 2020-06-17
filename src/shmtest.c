#include "types.h"
#include "user.h"

int
main(int argc, char* argv[])
{
    void *pte;
    printf(0, "create and share first page!\n");
    pte = (void *) shmget(1);
    if(pte != NULL) {
        printf(0, "\tcreate first page: success\n");
    } else {
        printf(0, "\tcreate first page: failure\n");
    }

    printf(0, "create and share second page!\n");
    pte = (void *) shmget(2);
    if(pte != NULL) {
        printf(0, "\tcreate second page: success\n");
    } else {
        printf(0, "\tcreate second page: failure\n");
    }
    
    printf(0, "share a duplicate page!\n");
    pte = (void *) shmget(1);
    if(pte != NULL) {
        printf(0, "\tfailure: true\n");
    } else {
        printf(0, "\tfailure: false\n");
    }

    return 0;
}