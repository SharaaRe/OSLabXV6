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
    if (lk->locked) {
        int i;
        for (i = NPROC; i >= 0; i--) {
            if (lk->queue[i] != 0 && lk->queue[i] <= pid)
                break;
            lk->queue[i + 1] = lk->queue[i];
        }
        lk->queue[i] = pid;
    }

    while (lk->locked && lk->queue[0] != pid)
        sleep(lk, &lk->lk);


    lk->locked = 1;
    lk->pid = pid;
    release(&lk->lk);
}


void
releaseticketlock(struct prioritylock* lk)
{
    acquire(&lk->lk);
    if (lk->pid != myproc()->pid)
        panic("invalid release!");

    for (int i = 1; i < NPROC; i++){
        if (lk->queue[i] == 0)
            break;
        lk->queue[i - 1] = lk->queue[i];
    }

    wakeup(lk); 
    release(&lk->lk);
}

