#!/usr/bin/env bash
set -euo pipefail

cleanup() {
    rm -f boot.o idt_asm.o idt.o pic.o pit.o kernel.o myos.elf
}
trap cleanup EXIT

CFLAGS="-m32 -ffreestanding -nostdlib -O2 -Wall -Wextra"

as --32 multiboot.s -o boot.o
as --32 idt.s -o idt_asm.o
gcc $CFLAGS -c idt.c -o idt.o
gcc $CFLAGS -c pic.c -o pic.o
gcc $CFLAGS -c pit.c -o pit.o
gcc $CFLAGS -c kernel.c -o kernel.o

ld -m elf_i386 -T linker.ld boot.o idt_asm.o idt.o pic.o pit.o kernel.o -o myos.elf

qemu-system-x86_64 -kernel myos.elf