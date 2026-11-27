
.set ALIGN,    1<<0
.set MEMINFO,  1<<1
.set FLAGS,    ALIGN | MEMINFO
.set MAGIC,    0x1BADB002
.set CHECKSUM, -(MAGIC + FLAGS)
.section .multiboot
.align 4
.long MAGIC
.long FLAGS
.long CHECKSUM

.section .bss
.align 16

.global boot_magic
boot_magic:
.long 0

.global boot_info
boot_info:
.long 0

.align 16
stack_bottom:
.skip 16384
stack_top:

.section .text
.global _start
.type _start, @function

_start:
    mov %eax, boot_magic
    mov %ebx, boot_info
    mov $stack_top, %esp
    call kmain

    cli
1:
    hlt
    jmp 1b
