(gdb) c
Continuing.
^C
Thread 2 received signal SIGINT, Interrupt.
0x80104550 in xchg (newval=<optimized out>, addr=<optimized out>) at x86.h:126
126	  asm volatile("lock; xchgl %0, %1" :
(gdb) where
#0  0x80104550 in xchg (newval=<optimized out>, addr=<optimized out>) at x86.h:126
#1  acquire (lk=<optimized out>) at spinlock.c:32
#2  0x80103f21 in yield () at proc.c:388
#3  0x801059cb in trap (tf=0x8dffef24) at trap.c:107
#4  0x801057bf in alltraps () at trapasm.S:20
#5  0x8dffef24 in ?? ()
#6  0x80104a49 in syscall () at syscall.c:139
#7  0x801059a5 in trap (tf=0x8dffefb4) at trap.c:43
#8  0x801057bf in alltraps () at trapasm.S:20
#9  0x8dffefb4 in ?? ()
Backtrace stopped: previous frame inner to this frame (corrupt stack?)
