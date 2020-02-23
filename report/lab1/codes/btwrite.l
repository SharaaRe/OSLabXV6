(gdb) break write 
Breakpoint 1 at 0x382: file usys.S, line 16.
(gdb) c
Continuing.
[Switching to Thread 2]

Thread 2 hit Breakpoint 1, write () at usys.S:16
16	SYSCALL(write)
(gdb) bt
#0  write () at usys.S:16
#1  0x000000b0 in cat (fd=3) at cat.c:13
#2  0x00000054 in main (argc=<optimized out>, argv=<optimized out>) at cat.c:39