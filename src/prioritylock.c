#include "types.h"
#include "defs.h"
#include "param.h"
#include "x86.h"
#include "memlayout.h"
#include "mmu.h"
#include "proc.h"
#include "spinlock.h"
#include "prioritylock.h"

void
initprioritylock(struct prioritylock* lk, char* name) {
    initlock(&lk->lk, "priority lock");
    lk->name = name;
    lk->pid = 0;
    lk->locked = 0;
}

void
acquirepriority(struct prioritylock *lk)
{
    acquire(&lk->lk);
    int pid = myproc()->pid;

    // add process to queue
    if (lk->locked) {
        int i;
        if (lk->queue[NPROC - 1] != 0)
            panic("full queue");

        for (i = NPROC - 1; i >= 0; i--) {
            if (lk->queue[i] == 0)
                continue;
                
            if (lk->queue[i] != 0 && lk->queue[i] <= pid)
                break;
            lk->queue[i + 1] = lk->queue[i];
        }
        lk->queue[i] = pid;
    } else {
        // add when queue is empty
        lk->queue[0] = pid;
    }

    while (lk->locked && lk->queue[0] != pid)
        sleep(lk, &lk->lk);


    lk->locked = 1;
    lk->pid = pid;
    release(&lk->lk);
}


void
releasepriority(struct prioritylock* lk)
{
    acquire(&lk->lk);
    if (lk->pid != myproc()->pid)
        panic("invalid release!");

    for (int i = 1; i < NPROC; i++){
        lk->queue[i - 1] = lk->queue[i];
        if (lk->queue[i] == 0)
            break;
    }
    if (lk->queue[0] == 0){
        lk->locked = 0;
        lk->pid = 0;
    }



    wakeup(lk); 
    release(&lk->lk);
}

void
printpriorityqueue(struct prioritylock* lk)
{
    acquire(&lk->lk);
    cprintf("priority queue:\n");
    for (int i = 0; i < NPROC && lk->queue[i]; i++){
        cprintf("%d->", lk->queue[i]);
    }
    cprintf("end\n");
    release(&lk->lk);
}

