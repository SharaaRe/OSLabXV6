objdump -D -M intel kernel | sed '/^.\{4,8\}:.*$\|^$\|^.*\.\.\..*$/d' > kernel.log

src/kernel:     file format elf32-i386
Disassembly of section .text:
80100000 <multiboot_header>:
8010000c <entry>:
80100040 <binit>:
801000d0 <bread>:
801001a0 <bwrite>:
801001e0 <brelse>:
...