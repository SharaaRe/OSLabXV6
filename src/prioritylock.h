#ifndef _PRIORITY_LOCK_
#define _PRIORITY_LOCK_


struct prioritylock
{
    int locked;
    struct spinlock lk; //spinlock protecting this prioritylock
    int queue[NPROC];
    
    //for debugging
    char* name;
    int pid; //process holding lock
};

void initprioritylock(struct prioritylock*, char*);
void acquirepriority(struct prioritylock*);
void releasepriority(struct prioritylock*);
void printpriorityqueue(struct prioritylock*);


#endif