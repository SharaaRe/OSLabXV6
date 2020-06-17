#include "types.h"
#include "user.h"

int
main(int argc, char* argv[])
{
    void *pte;
    pte = (void *) shmget(1);
    pte = (void *) shmget(2);

    pte = (void *) shmget(1);
    pte++;
    return 0;
}