#include "types.h"
#include "defs.h"
#include "param.h"
#include "memlayout.h"
#include "mmu.h"
#include "x86.h"
#include "proc.h"
#include "spinlock.h"

struct {
  struct spinlock lock;
  struct proc proc[NPROC];
} ptable, pptable;

static struct proc *initproc;

int nextpid = 1;
extern void forkret(void);
extern void trapret(void);

static void wakeup1(void *chan);

void
pinit(void)
{
  initlock(&ptable.lock, "ptable");
  initlock(&pptable.lock, "pptable");
}

// Must be called with interrupts disabled
int
cpuid() {
  return mycpu()-cpus;
}

// Must be called with interrupts disabled to avoid the caller being
// rescheduled between reading lapicid and running through the loop.
struct cpu*
mycpu(void)
{
  int apicid, i;
  
  if(readeflags()&FL_IF)
    panic("mycpu called with interrupts enabled\n");
  
  apicid = lapicid();
  // APIC IDs are not guaranteed to be contiguous. Maybe we should have
  // a reverse map, or reserve a register to store &cpus[i].
  for (i = 0; i < ncpu; ++i) {
    if (cpus[i].apicid == apicid)
      return &cpus[i];
  }
  panic("unknown apicid\n");
}

// Disable interrupts so that we are not rescheduled
// while reading proc from the cpu structure
struct proc*
myproc(void) {
  struct cpu *c;
  struct proc *p;
  pushcli();
  c = mycpu();
  p = c->proc;
  popcli();
  return p;
}

//PAGEBREAK: 32
// Look in the process table for an UNUSED proc.
// If found, change state to EMBRYO and initialize
// state required to run in the kernel.
// Otherwise return 0.
static struct proc*
allocproc(void)
{
  struct proc *p;
  char *sp;

  acquire(&ptable.lock);

  for(p = ptable.proc; p < &ptable.proc[NPROC]; p++)
    if(p->state == UNUSED)
      goto found;

  release(&ptable.lock);
  return 0;

found:
  p->state = EMBRYO;
  p->pid = nextpid++;

  release(&ptable.lock);

  // Allocate kernel stack.
  if((p->kstack = kalloc()) == 0){
    p->state = UNUSED;
    return 0;
  }
  sp = p->kstack + KSTACKSIZE;

  // Leave room for trap frame.
  sp -= sizeof *p->tf;
  p->tf = (struct trapframe*)sp;

  // Set up new context to start executing at forkret,
  // which returns to trapret.
  sp -= 4;
  *(uint*)sp = (uint)trapret;

  sp -= sizeof *p->context;
  p->context = (struct context*)sp;
  memset(p->context, 0, sizeof *p->context);
  p->context->eip = (uint)forkret;

  return p;
}

//PAGEBREAK: 32
// Set up first user process.
void
userinit(void)
{
  struct proc *p;
  extern char _binary_initcode_start[], _binary_initcode_size[];

  p = allocproc();
  
  initproc = p;
  if((p->pgdir = setupkvm()) == 0)
    panic("userinit: out of memory?");
  inituvm(p->pgdir, _binary_initcode_start, (int)_binary_initcode_size);
  p->sz = PGSIZE;
  memset(p->tf, 0, sizeof(*p->tf));
  p->tf->cs = (SEG_UCODE << 3) | DPL_USER;
  p->tf->ds = (SEG_UDATA << 3) | DPL_USER;
  p->tf->es = p->tf->ds;
  p->tf->ss = p->tf->ds;
  p->tf->eflags = FL_IF;
  p->tf->esp = PGSIZE;
  p->tf->eip = 0;  // beginning of initcode.S

  safestrcpy(p->name, "initcode", sizeof(p->name));
  p->cwd = namei("/");

  // this assignment to p->state lets other cores
  // run this process. the acquire forces the above
  // writes to be visible, and the lock is also needed
  // because the assignment might not be atomic.
  acquire(&ptable.lock);

  p->state = RUNNABLE;

  release(&ptable.lock);
}

// Grow current process's memory by n bytes.
// Return 0 on success, -1 on failure.
int
growproc(int n)
{
  uint sz;
  struct proc *curproc = myproc();

  sz = curproc->sz;
  if(n > 0){
    if((sz = allocuvm(curproc->pgdir, sz, sz + n)) == 0)
      return -1;
  } else if(n < 0){
    if((sz = deallocuvm(curproc->pgdir, sz, sz + n)) == 0)
      return -1;
  }
  curproc->sz = sz;
  switchuvm(curproc);
  return 0;
}

// Create a new process copying p as the parent.
// Sets up stack to return as if from system call.
// Caller must set state of returned proc to RUNNABLE.
int
fork(void)
{
  int i, pid;
  struct proc *np;
  struct proc *curproc = myproc();
  struct _sysclog *s;

  // Allocate process.
  if((np = allocproc()) == 0){
    return -1;
  }

  // Copy process state from proc.
  if((np->pgdir = copyuvm(curproc->pgdir, curproc->sz)) == 0){
    kfree(np->kstack);
    np->kstack = 0;
    np->state = UNUSED;
    return -1;
  }
  np->sz = curproc->sz;
  np->parent = curproc;
  *np->tf = *curproc->tf;

  // Clear %eax so that fork returns 0 in the child.
  np->tf->eax = 0;

  for(i = 0; i < NOFILE; i++)
    if(curproc->ofile[i])
      np->ofile[i] = filedup(curproc->ofile[i]);
  np->cwd = idup(curproc->cwd);

  safestrcpy(np->name, curproc->name, sizeof(curproc->name));

  pid = np->pid;

  acquire(&ptable.lock);

  np->state = RUNNABLE;

  for(s = np->sysclog; s < &np->sysclog[NLOGPAIR]; s++) {
    s->callno = 0;
    s->retval = 0;
  }

  release(&ptable.lock);

  return pid;
}

// Exit the current process.  Does not return.
// An exited process remains in the zombie state
// until its parent calls wait() to find out it exited.
void
exit(void)
{
  struct proc *curproc = myproc();
  struct proc *p;
  int fd;
  
  int npptable, npair = 0;
  struct _sysclog *s;

  if(curproc == initproc)
    panic("init exiting");

  // Close all open files.
  for(fd = 0; fd < NOFILE; fd++){
    if(curproc->ofile[fd]){
      fileclose(curproc->ofile[fd]);
      curproc->ofile[fd] = 0;
    }
  }

  begin_op();
  iput(curproc->cwd);
  end_op();
  curproc->cwd = 0;

  acquire(&ptable.lock);

  // Parent might be sleeping in wait().
  wakeup1(curproc->parent);

  // Pass abandoned children to init.
  for(p = ptable.proc; p < &ptable.proc[NPROC]; p++){
    if(p->parent == curproc){
      p->parent = initproc;
      if(p->state == ZOMBIE)
        wakeup1(initproc);
    }
  }

  acquire(&pptable.lock);

  for(npptable = 0; npptable < NPROC; npptable++) {
    if(pptable.proc[npptable].pid == 0) break;
  }

  pptable.proc[npptable].pid = curproc->pid;

  for(s = curproc->sysclog; s < &curproc->sysclog[NLOGPAIR]; s++) {
    if(s->callno != 0) {
      pptable.proc[npptable].sysclog[npair].callno = s->callno;
      pptable.proc[npptable].sysclog[npair].retval = s->retval;
      npair++;
    }
  }

  release(&pptable.lock);

  // Jump into the scheduler, never to return.
  curproc->state = ZOMBIE;
  sched();
  panic("zombie exit");
}

// Wait for a child process to exit and return its pid.
// Return -1 if this process has no children.
int
wait(void)
{
  struct proc *p;
  int havekids, pid;
  struct proc *curproc = myproc();
  
  acquire(&ptable.lock);
  for(;;){
    // Scan through table looking for exited children.
    havekids = 0;
    for(p = ptable.proc; p < &ptable.proc[NPROC]; p++){
      if(p->parent != curproc)
        continue;
      havekids = 1;
      if(p->state == ZOMBIE){
        // Found one.
        pid = p->pid;
        kfree(p->kstack);
        p->kstack = 0;
        freevm(p->pgdir);
        p->pid = 0;
        p->parent = 0;
        p->name[0] = 0;
        p->killed = 0;
        p->state = UNUSED;
        release(&ptable.lock);
        return pid;
      }
    }

    // No point waiting if we don't have any children.
    if(!havekids || curproc->killed){
      release(&ptable.lock);
      return -1;
    }

    // Wait for children to exit.  (See wakeup1 call in proc_exit.)
    sleep(curproc, &ptable.lock);  //DOC: wait-sleep
  }
}

//PAGEBREAK: 42
// Per-CPU process scheduler.
// Each CPU calls scheduler() after setting itself up.
// Scheduler never returns.  It loops, doing:
//  - choose a process to run
//  - swtch to start running that process
//  - eventually that process transfers control
//      via swtch back to the scheduler.
void
scheduler(void)
{
  struct proc *p;
  struct cpu *c = mycpu();
  c->proc = 0;
  
  for(;;){
    // Enable interrupts on this processor.
    sti();

    // Loop over process table looking for process to run.
    acquire(&ptable.lock);
    for(p = ptable.proc; p < &ptable.proc[NPROC]; p++){
      if(p->state != RUNNABLE)
        continue;

      // Switch to chosen process.  It is the process's job
      // to release ptable.lock and then reacquire it
      // before jumping back to us.
      c->proc = p;
      switchuvm(p);
      p->state = RUNNING;
      check_alarm(p);
      swtch(&(c->scheduler), p->context);
      switchkvm();

      // Process is done running for now.
      // It should have changed its p->state before coming back.
      c->proc = 0;
    }
    release(&ptable.lock);

  }
}

// Enter scheduler.  Must hold only ptable.lock
// and have changed proc->state. Saves and restores
// intena because intena is a property of this
// kernel thread, not this CPU. It should
// be proc->intena and proc->ncli, but that would
// break in the few places where a lock is held but
// there's no process.
void
sched(void)
{
  int intena;
  struct proc *p = myproc();

  if(!holding(&ptable.lock))
    panic("sched ptable.lock");
  if(mycpu()->ncli != 1)
    panic("sched locks");
  if(p->state == RUNNING)
    panic("sched running");
  if(readeflags()&FL_IF)
    panic("sched interruptible");
  intena = mycpu()->intena;
  swtch(&p->context, mycpu()->scheduler);
  mycpu()->intena = intena;
}

// Give up the CPU for one scheduling round.
void
yield(void)
{
  acquire(&ptable.lock);  //DOC: yieldlock
  myproc()->state = RUNNABLE;
  sched();
  release(&ptable.lock);
}

// A fork child's very first scheduling by scheduler()
// will swtch here.  "Return" to user space.
void
forkret(void)
{
  static int first = 1;
  // Still holding ptable.lock from scheduler.
  release(&ptable.lock);

  if (first) {
    // Some initialization functions must be run in the context
    // of a regular process (e.g., they call sleep), and thus cannot
    // be run from main().
    first = 0;
    iinit(ROOTDEV);
    initlog(ROOTDEV);
  }

  // Return to "caller", actually trapret (see allocproc).
}

// Atomically release lock and sleep on chan.
// Reacquires lock when awakened.
void
sleep(void *chan, struct spinlock *lk)
{
  struct proc *p = myproc();
  
  if(p == 0)
    panic("sleep");

  if(lk == 0)
    panic("sleep without lk");

  // Must acquire ptable.lock in order to
  // change p->state and then call sched.
  // Once we hold ptable.lock, we can be
  // guaranteed that we won't miss any wakeup
  // (wakeup runs with ptable.lock locked),
  // so it's okay to release lk.
  if(lk != &ptable.lock){  //DOC: sleeplock0
    acquire(&ptable.lock);  //DOC: sleeplock1
    release(lk);
  }
  // Go to sleep.
  p->chan = chan;
  p->state = SLEEPING;

  sched();

  // Tidy up.
  p->chan = 0;

  // Reacquire original lock.
  if(lk != &ptable.lock){  //DOC: sleeplock2
    release(&ptable.lock);
    acquire(lk);
  }
}

//PAGEBREAK!
// Wake up all processes sleeping on chan.
// The ptable lock must be held.
static void
wakeup1(void *chan)
{
  struct proc *p;

  for(p = ptable.proc; p < &ptable.proc[NPROC]; p++)
    if(p->state == SLEEPING && p->chan == chan)
      p->state = RUNNABLE;
}

// Wake up all processes sleeping on chan.
void
wakeup(void *chan)
{
  acquire(&ptable.lock);
  wakeup1(chan);
  release(&ptable.lock);
}

// Kill the process with the given pid.
// Process won't exit until it returns
// to user space (see trap in trap.c).
int
kill(int pid)
{
  struct proc *p;
  int npair = 0;
  struct _sysclog *s;
  int npptable;

  acquire(&ptable.lock);
  for(p = ptable.proc; p < &ptable.proc[NPROC]; p++){
    if(p->pid == pid){
      p->killed = 1;
      // Wake process from sleep if necessary.
      if(p->state == SLEEPING)
        p->state = RUNNABLE;

      for(s = p->sysclog; s < &p->sysclog[NLOGPAIR]; s++) {
        if(s->callno != 0) {
          initproc->sysclog[npair].callno = s->callno;
          initproc->sysclog[npair].retval = s->retval;
          npair++;
        }
      }

      acquire(&pptable.lock);
      for(npptable = 0; npptable < NPROC; npptable++) {
        if(pptable.proc[npptable].pid == 0) break;
      }

      pptable.proc[npptable].pid = p->pid;

      for(s = p->sysclog; s < &p->sysclog[NLOGPAIR]; s++) {
        if(s->callno != 0) {
          pptable.proc[npptable].sysclog[npair].callno = s->callno;
          pptable.proc[npptable].sysclog[npair].retval = s->retval;
          npair++;
        }
      }

      release(&pptable.lock);
      release(&ptable.lock);
      return 0;
    }
  }

  release(&ptable.lock);
  return -1;
}

//PAGEBREAK: 36
// Print a process listing to console.  For debugging.
// Runs when user types ^P on console.
// No lock to avoid wedging a stuck machine further.
void
procdump(void)
{
  static char *states[] = {
  [UNUSED]    "unused",
  [EMBRYO]    "embryo",
  [SLEEPING]  "sleep ",
  [RUNNABLE]  "runble",
  [RUNNING]   "run   ",
  [ZOMBIE]    "zombie"
  };
  int i;
  struct proc *p;
  char *state;
  uint pc[10];

  for(p = ptable.proc; p < &ptable.proc[NPROC]; p++){
    if(p->state == UNUSED)
      continue;
    if(p->state >= 0 && p->state < NELEM(states) && states[p->state])
      state = states[p->state];
    else
      state = "???";
    cprintf("%d %s %s", p->pid, state, p->name);
    if(p->state == SLEEPING){
      getcallerpcs((uint*)p->context->ebp+2, pc);
      for(i=0; i<10 && pc[i] != 0; i++)
        cprintf(" %p", pc[i]);
    }
    cprintf("\n");
  }
}

// return count of a number digits
int count_num_of_digits() 
{
  int n = myproc()->tf->ebx;
  int copy = n;
  int count = 0;
  while (n > 0) {
    count++;
    n = n / 10;
  }
  cprintf("count of number %d digits: %d \n", copy, count);

  return count;
}


// sets alarm and waits untill the proper time
// has left and then warn the user
// time is in milisecond unit
void 
set_alarm(int time) 
{
  if (time <= 0) {
    cprintf("time should be bigger than zero, no alarm is set!\n");
    return;
  }
  acquire(&tickslock);
  myproc()->alarm_time = ticks + time / 10;
  release(&tickslock);
}

void
check_alarm(struct proc* p)
{
  if (ticks >= p->alarm_time && p->alarm_time > 0)
  {
    cprintf("Alarm!!!\n");
    p->alarm_time = 0;
  }
} 



// prints all system calls and the their return values
// in each process
int
print_syscalls(void)
{
  struct proc *p;
  struct _sysclog *s;
  const char *names[] = {"", "fork", "exit", "wait", "pipe", "read",
    "kill", "exec", "fstat", "chdir", "dup", "getpid", "sbrk", "sleep",
    "uptime", "open", "write", "mknod", "unlink", "link", "mkdir",
    "close", "count_num_of_digits", "set_alarm", "print_syscalls",
    "set_edx", "read_registers"};

  acquire(&ptable.lock);
  for(p = ptable.proc; p < &ptable.proc[NPROC]; p++) {
    if(p->pid == 0) continue;
    cprintf("Proc %d\n", p->pid);
    for(s = p->sysclog; s < &p->sysclog[NLOGPAIR]; s++) {
      if(s->callno == 0) break;
      if(s->callno > sizeof(names)/sizeof(names[0])) {
        cprintf("\t%s:\t%d\n", "unknown", s->retval);
      } else {
        cprintf("\t%s:\t%d\n", names[s->callno], s->retval);
      }
    }
  }
  release(&ptable.lock);

  cprintf("Terminated Processes: \n");

  acquire(&pptable.lock);
  for(p = pptable.proc; p < &pptable.proc[NPROC]; p++) {
    if(p->pid == 0) continue;
    cprintf("Proc %d\n", p->pid);
    for(s = p->sysclog; s < &p->sysclog[NLOGPAIR]; s++) {
      if(s->callno == 0) break;
      if(s->callno > sizeof(names)/sizeof(names[0])) {
        cprintf("\t%s:\t%d\n", "unknown", s->retval);
      } else {
        cprintf("\t%s:\t%d\n", names[s->callno], s->retval);
      }
    }
  }
  release(&pptable.lock);

  return 23;
}

int
count_syscalls(void)
{
  int n_syscalls_per_cpu = 0, i = 0;

  struct cpu c;

  for (c = cpus[0]; i < NCPU; ++i) {
    n_syscalls_per_cpu += c.n_syscalls;
  }

  cprintf("per cpu sum: %d\n", n_syscalls_per_cpu);

  return g_n_syscalls;
}


// simply set edx register to value
int
set_edx(int value) {
  acquire(&ptable.lock);
  struct proc *p = myproc();
  p->tf->edx = value;

  release(&ptable.lock);
  return 25;
}

// print registers
int
read_registers(void) {
  // unsigned int v;
  acquire(&ptable.lock);
  struct proc *p = myproc();

  // asm("movl %%eax, %0" : "=r"(v) :);
  // cprintf("\x1B[32mEAX\x1B[37m:  \x1B[36m0x%x\x1B[0m\n", v);
  cprintf("\x1B[32mEAX\x1B[37m:  \x1B[36m0x%x\x1B[0m\n", p->tf->eax);

  // asm("movl %%ebx, %0" : "=r"(v) :);
  // cprintf("\x1B[32mEBX\x1B[37m:  \x1B[36m0x%x\x1B[0m\n", v);
  cprintf("\x1B[32mEBX\x1B[37m:  \x1B[36m0x%x\x1B[0m\n", p->tf->ebx);

  // asm("movl %%ecx, %0" : "=r"(v) :);
  // cprintf("\x1B[32mECX\x1B[37m:  \x1B[36m0x%x\x1B[0m\n", v);
  cprintf("\x1B[32mECX\x1B[37m:  \x1B[36m0x%x\x1B[0m\n", p->tf->ecx);

  // asm("movl %%edx, %0" : "=r"(v) :);
  // cprintf("\x1B[32mEDX\x1B[37m:  \x1B[36m0x%x\x1B[0m\n", v);
  cprintf("\x1B[32mEDX\x1B[37m:  \x1B[36m0x%x\x1B[0m\n", p->tf->edx);

  // asm("movl %%esi, %0" : "=r"(v) :);
  // cprintf("\x1B[32mESI\x1B[37m:  \x1B[36m0x%x\x1B[0m\n", v);
  cprintf("\x1B[32mESI\x1B[37m:  \x1B[36m0x%x\x1B[0m\n", p->tf->esi);

  // asm("movl %%edi, %0" : "=r"(v) :);
  // cprintf("\x1B[32mEDI\x1B[37m:  \x1B[36m0x%x\x1B[0m\n", v);
  cprintf("\x1B[32mEDI\x1B[37m:  \x1B[36m0x%x\x1B[0m\n", p->tf->edi);

  // asm("movl %%ebp, %0" : "=r"(v) :);
  // cprintf("\x1B[32mEBP\x1B[37m:  \x1B[36m0x%x\x1B[0m\n", v);
  cprintf("\x1B[32mEBP\x1B[37m:  \x1B[36m0x%x\x1B[0m\n", p->tf->ebp);

  // asm("movl %%esp, %0" : "=r"(v) :);
  // cprintf("\x1B[32mESP\x1B[37m:  \x1B[36m0x%x\x1B[0m\n", v);
  cprintf("\x1B[32mESP\x1B[37m:  \x1B[36m0x%x\x1B[0m\n", p->tf->esp);

  // asm("movl %%eip, %0" : "=r"(v) :);
  cprintf("\x1B[32mEIP\x1B[37m:  \x1B[36m0x%x\x1B[0m\n", p->tf->eip);
  // cprintf("EIP:  0x%x\n", p->tf->eip);

  // asm("movl %%eflags, %0" : "=r"(v) :);
  cprintf("\x1B[32mEFLAGS\x1B[37m:  \x1B[36m0x%x\x1B[0m\n", p->tf->eflags);
  
  cprintf("\x1B[32mSS\x1B[37m:  \x1B[36m0x%x\x1B[0m\n", p->tf->ss);
  cprintf("\x1B[32mDS\x1B[37m:  \x1B[36m0x%x\x1B[0m\n", p->tf->ds);
  cprintf("\x1B[32mES\x1B[37m:  \x1B[36m0x%x\x1B[0m\n", p->tf->es);
  cprintf("\x1B[32mFS\x1B[37m:  \x1B[36m0x%x\x1B[0m\n", p->tf->fs);
  cprintf("\x1B[32mGS\x1B[37m:  \x1B[36m0x%x\x1B[0m\n", p->tf->gs);
  
  release(&ptable.lock);
  return 26;
}


// returns state of a process by its pid
// return -1 if the pid is not found
int
get_state(int pid) 
{
  struct proc *p;
  int state = -1;
  acquire(&ptable.lock);
  for(p = ptable.proc; p < &ptable.proc[NPROC]; p++) {
    if(p->pid == pid) {
      state = p->state;
      break;
    }
  }
  release(&ptable.lock);
  return state;
}