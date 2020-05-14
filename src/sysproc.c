#include "types.h"
#include "x86.h"
#include "defs.h"
#include "date.h"
#include "param.h"
#include "memlayout.h"
#include "mmu.h"
#include "proc.h"

int
sys_fork(void)
{
  return fork();
}

int
sys_exit(void)
{
  exit();
  return 0;  // not reached
}

int
sys_wait(void)
{
  return wait();
}

int
sys_kill(void)
{
  int pid;

  if(argint(0, &pid) < 0)
    return -1;
  return kill(pid);
}

int
sys_getpid(void)
{
  return myproc()->pid;
}

int
sys_sbrk(void)
{
  int addr;
  int n;

  if(argint(0, &n) < 0)
    return -1;
  addr = myproc()->sz;
  if(growproc(n) < 0)
    return -1;
  return addr;
}

int
sys_sleep(void)
{
  int n;
  uint ticks0;

  if(argint(0, &n) < 0)
    return -1;
  acquire(&tickslock);
  ticks0 = ticks;
  while(ticks - ticks0 < n){
    if(myproc()->killed){
      release(&tickslock);
      return -1;
    }
    sleep(&ticks, &tickslock);
  }
  release(&tickslock);
  return 0;
}

// return how many clock tick interrupts have occurred
// since start.
int
sys_uptime(void)
{
  uint xticks;

  acquire(&tickslock);
  xticks = ticks;
  release(&tickslock);
  return xticks;
}

// return cout number of digits
int 
sys_count_num_of_digits(void)
{
  return count_num_of_digits();
}

int
sys_set_alarm(void) 
{
  int n;
  if(argint(0, &n) < 0)
    return -1;
  set_alarm(n);
  return 0;
}


// print outs all system calls of each process
int
sys_print_syscalls(void)
{
  return print_syscalls();
}

// set edx register value
int
sys_set_edx(void) {
  int value;
  if (argint(0, &value) < 0)
    return -1;
  return set_edx(value);
}

// read and print registers
int
sys_read_registers(void) {
  return read_registers();
}

// print processes
int
sys_ps ( void ) {
  return ps();
}

//set process queue
int
sys_set_queue(void) {
  int value;
  if (argint(0, &value) < 0)
    return -1;
  return set_queue(value);
}