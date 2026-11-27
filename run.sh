#!/usr/bin/env bash
set -euo pipefail

mkdir -p build

DISK=$(mktemp /tmp/glorious-XXXXXX.img)
truncate -s 16M "$DISK"

ISO_DIR="build/iso"

cleanup() {
    rm -rf build
    rm -f gloriousos.elf
    rm -f "$DISK"
}

trap cleanup EXIT

# compiler flags
CFLAGS="-m32 -march=i386 -mno-sse -mno-sse2 -mno-mmx -mno-3dnow -mno-avx -mno-avx2 \
        -ffreestanding -fno-builtin -fno-stack-protector -fno-pie -nostdlib \
        -O2 -Wall -Wextra \
        -Ilibs/util -Ikernel -Idrivers -Idrivers/disk -Ilibs/gfs"

echo "[BUILD] assembling bootloader..."
as --32 boot/multiboot.s -o build/boot.o

echo "[BUILD] assembling IDT..."
as --32 kernel/idt.s -o build/idt_asm.o
echo "[BUILD] compiling mm.."
gcc $CFLAGS -c libs/mm/heap.c -o build/heap.o
echo "[BUILD] compiling utilities..."
gcc $CFLAGS -c libs/util/utils.c -o build/utils.o
gcc $CFLAGS -c libs/util/stdtypes.c -o build/stdtypes.o
gcc $CFLAGS -c libs/util/log.c -o build/log.o
gcc $CFLAGS -c libs/util/info.c -o build/info.o
echo "[BUILD] compiling kernel subsystems..."
gcc $CFLAGS -c kernel/idt.c -o build/idt.o
gcc $CFLAGS -c kernel/pic.c -o build/pic.o
gcc $CFLAGS -c kernel/pit.c -o build/pit.o
gcc $CFLAGS -c kernel/programs.c -o build/programs.o

echo "[BUILD] compiling drivers..."
gcc $CFLAGS -c drivers/display.c -o build/display.o
gcc $CFLAGS -c drivers/keyboard.c -o build/keyboard.o
gcc $CFLAGS -c drivers/disk/hata.c -o build/ata.o

echo "[BUILD] compiling filesystem..."
gcc $CFLAGS -c libs/gfs/gfs.c -o build/gfs.o
echo "[BUILD] compiling programs..."
PROGRAMS_DIR="kernel/programs"
PROGRAMS_BUILD_DIR="build/programs"
mkdir -p "$PROGRAMS_BUILD_DIR"

PROGRAM_OBJECTS=()

while IFS= read -r -d '' file; do
    relative="${file#"$PROGRAMS_DIR"/}"
    object="$PROGRAMS_BUILD_DIR/${relative%.c}.o"
    mkdir -p "$(dirname "$object")"
    echo "[BUILD] compiling program: $file"
    gcc $CFLAGS -c "$file" -o "$object"
    PROGRAM_OBJECTS+=("$object")

done < <(find "$PROGRAMS_DIR" -type f -name "*.c" -print0)

echo "[BUILD] compiled ${#PROGRAM_OBJECTS[@]} program files"
echo "[BUILD] compiling kernel..."
gcc $CFLAGS -c kernel/kernel.c -o build/kernel.o

echo "[BUILD] linking kernel..."
ld -m elf_i386 -z max-page-size=0x1000 \
    -T kernel/linker.ld \
    -o gloriousos.elf \
    build/boot.o \
    build/idt_asm.o \
    build/heap.o \
    build/utils.o \
    build/stdtypes.o \
    build/log.o \
    build/info.o \
    build/idt.o \
    build/pic.o \
    build/pit.o \
    build/programs.o \
    build/display.o \
    build/keyboard.o \
    build/ata.o \
    build/gfs.o \
    build/kernel.o \
    "${PROGRAM_OBJECTS[@]}"
echo "[CHECK] validating Multiboot header..."

if grub-file --is-x86-multiboot gloriousos.elf; then
    echo "[CHECK] Multiboot header is valid!"
else
    echo "[ERROR] Multiboot header validation FAILED!"
    echo "[ERROR] grub-file returned exit code $?"
    echo "[DEBUG] ELF details:"
    readelf -h gloriousos.elf
    echo "[DEBUG] Multiboot section:"
    readelf -S gloriousos.elf | grep -E 'multiboot|text'
    exit 1
fi

echo "[BUILD] preparing ISO structure..."

mkdir -p "$ISO_DIR/boot/grub"
cp gloriousos.elf "$ISO_DIR/boot/gloriousos.elf"

cat > "$ISO_DIR/boot/grub/grub.cfg" << 'EOF'
set timeout=0
set default=0

menuentry "GloriousOS" {
    multiboot /boot/gloriousos.elf
    boot
}
EOF

echo "[BUILD] generating bootable GRUB ISO..."
grub-mkrescue -o gloriousOS.iso "$ISO_DIR"

echo "[BUILD] ISO successfully created!"
echo "[QEMU] launching gloriousOS..."
echo "[QEMU] booting from GRUB ISO"
echo "[QEMU] temp disk: $DISK"
echo "[QEMU] disk size: 16MB"

qemu-system-i386 \
    -machine pc \
    -m 16M \
    -cdrom gloriousOS.iso \
    -boot d \
    -drive file="$DISK",format=raw,if=ide,index=0 \
    -serial stdio \
    -monitor none \
    -no-reboot \
    -no-shutdown \
    -d int,cpu_reset -D /tmp/qemu.log