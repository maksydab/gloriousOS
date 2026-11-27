
#include "info.h"

extern unsigned int boot_magic;
extern unsigned int boot_info;

static void cpuid(
    unsigned int leaf,
    unsigned int subleaf,
    unsigned int *a,
    unsigned int *b,
    unsigned int *c,
    unsigned int *d
) {
    asm volatile (
        "cpuid"
        : "=a"(*a), "=b"(*b), "=c"(*c), "=d"(*d)
        : "a"(leaf), "c"(subleaf)
    );
}

void cpu_get_vendor(char *vendor) {
    unsigned int a, b, c, d;

    cpuid(0, 0, &a, &b, &c, &d);

    *(unsigned int *)(vendor)     = b;
    *(unsigned int *)(vendor + 4) = d;
    *(unsigned int *)(vendor + 8) = c;

    vendor[12] = '\0';
}


void cpu_get_brand(char *brand) {
    unsigned int a, b, c, d;

    cpuid(0x80000000, 0, &a, &b, &c, &d);

    if (a < 0x80000004) {
        brand[0] = '\0';
        return;
    }

    unsigned int *ptr = (unsigned int *)brand;

    for (unsigned int i = 0; i < 3; i++) {
        cpuid(0x80000002 + i, 0, &a, &b, &c, &d);

        *ptr++ = a;
        *ptr++ = b;
        *ptr++ = c;
        *ptr++ = d;
    }

    brand[48] = '\0';
}


unsigned int cpu_get_cores(void) {
    unsigned int a, b, c, d;

    cpuid(1, 0, &a, &b, &c, &d);

    return (b >> 16) & 0xFF;
}


typedef struct {
    unsigned int flags;
    unsigned int mem_lower;
    unsigned int mem_upper;
} __attribute__((packed)) MultibootInfo;

unsigned int ram_get_total_kb(void) {
    if (boot_magic != 0x2BADB002)
        return 0;

    if (!boot_info)
        return 0;

    MultibootInfo *mbi = (MultibootInfo *)boot_info;

    if (!(mbi->flags & 1))
        return 0;

    return mbi->mem_lower + mbi->mem_upper;
}

unsigned int ram_get_total_mb(void) {
    return ram_get_total_kb() / 1024;
}

unsigned int info_get_boot_magic(void) {
    return boot_magic;
}
