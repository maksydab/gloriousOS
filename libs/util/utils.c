
#include "utils.h"

void *memset(void *dest, int value, size_t count) {
    unsigned char *ptr = dest;

    for (size_t i = 0; i < count; i++) {
        ptr[i] = (unsigned char)value;
    }

    return dest;
}

void *memcpy(void *dest, const void *src, size_t count) {
    unsigned char *d = dest;
    const unsigned char *s = src;

    for (size_t i = 0; i < count; i++) {
        d[i] = s[i];
    }

    return dest;
}

int memcmp(const void *a, const void *b, size_t count) {
    const unsigned char *x = a;
    const unsigned char *y = b;

    for (size_t i = 0; i < count; i++) {
        if (x[i] != y[i]) {
            return x[i] - y[i];
        }
    }

    return 0;
}

size_t strlen(const char *str) {
    size_t len = 0;

    while (str[len] != '\0') {
        len++;
    }

    return len;
}
