objdump -D -M intel src/_cat | sed '/^.\{4,8\}:.*$\|^$\|^.*\.\.\..*$/d' > _cat.log

Disassembly of section .text:
00000000 <main>:
00000090 <cat>:
00000110 <strcpy>:
00000140 <strcmp>:
00000190 <strlen>:
000001c0 <memset>:
000001e0 <strchr>: