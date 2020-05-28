#ifndef _PRIORITY_LOCK_
#define _PRIORITY_LOCK_

#include "spinlock.h"

struct prioritylock
{
    int locked;
    struct spinlock lk; //spinlock protecting this prioritylock
    int queue[NPROC];
    
    //for debugging
    char* name;
    int pid; //process holding lock
};




#endif