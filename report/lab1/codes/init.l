$ make qemu-gdb
*** Now run 'gdb'.
qemu-system-i386 -serial mon:stdio -drive file=fs.img,index=1,media=disk,format=raw -drive file=xv6.img,index=0,media=disk,format=raw -smp 2 -m 512  -S -gdb tcp::26000
xv6...
cpu1: starting 1
cpu0: starting 0
sb: size 1000 nblocks 941 ninodes 200 nlog 30 logstart 2 inodestart 32 bmap start 58
--------------------------------- 
--------- Group Members --------- 
- 1. Mohammad Rabiei: 810195395 - 
- 2. Sharare Norouzi: 810097015 - 
- 3. Ali Ranjbar    : 810097029 - 
--------------------------------- 
init: starting sh
$ 