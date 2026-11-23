
#!/usr/bin/env bash
set -euo pipefail

mkdir -p build

cleanup() {
    rm -rf build
    rm -f myos.elf
}

trap cleanup EXIT

CFLAGS="-m32 -ffreestanding -fno-builtin -fno-stack-protector -fno-pie -nostdlib -O2 -Wall -Wextra -Ilibs/util -Ikernel -Idrivers"

as --32 boot/multiboot.s -o build/boot.o

as --32 kernel/idt.s -o build/idt_asm.o

gcc $CFLAGS -c libs/util/utils.c -o build/utils.o

gcc $CFLAGS -c kernel/idt.c -o build/idt.o
gcc $CFLAGS -c kernel/pic.c -o build/pic.o
gcc $CFLAGS -c kernel/pit.c -o build/pit.o

gcc $CFLAGS -c drivers/display.c -o build/display.o
gcc $CFLAGS -c drivers/keyboard.c -o build/keyboard.o
gcc $CFLAGS -c libs/gfs/gfs.c -o build/gfs.o
gcc $CFLAGS -c kernel/kernel.c -o build/kernel.o

ld -m elf_i386 \
    -T kernel/linker.ld \
    -o myos.elf \
    build/boot.o \
    build/idt_asm.o \
    build/utils.o \
    build/idt.o \
    build/pic.o \
    build/pit.o \
    build/display.o \
    build/keyboard.o \
    build/kernel.o

echo "build successful!"

# run it glrouiously
qemu-system-x86_64 -kernel myos.elf
