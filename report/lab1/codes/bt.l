(gdb) c
Continuing.

[Switching to Thread 2]

Thread 2 hit Breakpoint 1, read () at usys.S:15
15	SYSCALL(read)
(gdb) bt
#0  read () at usys.S:15
#1  0x000000ca in cat (fd=3) at cat.c:12
#2  0x00000054 in main (argc=<optimized out>, argv=<optimized out>) at cat.c:39

(gdb) help bt
Print backtrace of all stack frames, or innermost COUNT frames.
With a negative argument, print outermost -COUNT frames.
Use of the 'full' qualifier also prints the values of the local variables.
Use of the 'no-filters' qualifier prohibits frame filters from executing
on this backtrace.