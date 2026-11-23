#!/usr/bin/env bash
set -euo pipefail

cleanup() {
    rm -f boot.o kernel.o myos.elf
}
trap cleanup EXIT

as --32 multiboot.s -o boot.o
gcc -m32 -ffreestanding -nostdlib -O2 -Wall -Wextra -c kernel.c -o kernel.o
ld -m elf_i386 -T linker.ld boot.o kernel.o -o myos.elf

qemu-system-x86_64 -kernel myos.elf