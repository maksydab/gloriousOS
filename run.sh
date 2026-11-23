
#!/usr/bin/env bash
set -euo pipefail

mkdir -p build

DISK=$(mktemp /tmp/glorious-XXXXXX.img)
truncate -s 16M "$DISK"

cleanup() {
    rm -rf build
    rm -f myos.elf
    rm -f "$DISK"
}

trap cleanup EXIT

# sie compiler flags
CFLAGS="-m32 -ffreestanding -fno-builtin -fno-stack-protector -fno-pie -nostdlib -O2 -Wall -Wextra -Ilibs/util -Ikernel -Idrivers -Idrivers/disk -Ilibs/gfs"

echo "[BUILD] assembling boots-loader..."

as --32 boot/multiboot.s -o build/boot.o

echo "[BUILD] Assembling the assembler..."

as --32 kernel/idt.s -o build/idt_asm.o

echo "[BUILD] compiling utilities..."

gcc $CFLAGS -c libs/util/utils.c -o build/utils.o

echo "[BUILD] compiling kernel subsystems..."

gcc $CFLAGS -c kernel/idt.c -o build/idt.o
gcc $CFLAGS -c kernel/pic.c -o build/pic.o
gcc $CFLAGS -c kernel/pit.c -o build/pit.o

echo "[BUILD] compiling drivers..."

gcc $CFLAGS -c drivers/display.c -o build/display.o
gcc $CFLAGS -c drivers/keyboard.c -o build/keyboard.o
gcc $CFLAGS -c drivers/disk/hata.c -o build/ata.o

echo "[BUILD] compiling filesystem..."

gcc $CFLAGS -c libs/gfs/gfs.c -o build/gfs.o

echo "[BUILD] compiling the kernel ..."

gcc $CFLAGS -c kernel/kernel.c -o build/kernel.o

echo "[BUILD] linking it..."

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
    build/ata.o \
    build/gfs.o \
    build/kernel.o

echo "[BUILD] build was successful!"
echo "[QEMU] launching gloriousOS..."
echo "[QEMU] temp disk $DISK"
echo "[QEMU] disc size: 16MB"

qemu-system-x86_64 \
    -kernel myos.elf \
    -drive file="$DISK",format=raw,if=ide,index=0
