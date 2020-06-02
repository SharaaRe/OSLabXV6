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
    int pid = myproc()->pid;
    acquire(&lk->lk);

    // add process to queue
    if (lk->locked) {
        int i;
        if (lk->queue[NPROC - 1] != 0)
            panic("full queue");

        for (i = NPROC - 1; i > 0; i--) {
            if (lk->queue[i - 1] == 0)
                continue;
                
            lk->queue[i] = lk->queue[i - 1];
            if (lk->queue[i - 1] != 0 && lk->queue[i - 1] <= pid)
                break;
        }
        lk->queue[i] = pid;
    } else {
        // add when queue is empty
        lk->queue[0] = pid;
    }

    // print queue after enqueue
    cprintf("priority queue after enqueue of %d:\n", pid);
    for (int i = 0; i < NPROC && lk->queue[i]; i++){
        cprintf("%d->", lk->queue[i]);
    }
    cprintf("end\n");

    while (lk->locked && lk->pid != pid) {
        sleep(lk, &lk->lk);
    }

    
    lk->locked = 1;
    lk->pid = pid;
    release(&lk->lk);
}


void
releasepriority(struct prioritylock* lk)
{
    int pid = myproc()->pid;
    int valid_head = 0;
    acquire(&lk->lk);
    if (lk->pid != pid)
        panic("invalid release!");

    while(!valid_head) {
        if(lk->queue[0] == 0 || get_state(lk->queue[0]) == SLEEPING) {
            valid_head = 1;
            lk->pid = lk->queue[0];
        }

        for (int i = 1; i < NPROC; i++){
            lk->queue[i - 1] = lk->queue[i];
            if (lk->queue[i] == 0)
                break;
        }
    }

    if (lk->pid == 0){
        lk->locked = 0;
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

