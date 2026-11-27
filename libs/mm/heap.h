
#ifndef GLORIOUS_HEAP_H
#define GLORIOUS_HEAP_H

#include <stddef.h>
#include <stdbool.h>

// thank you chatgpt
bool heap_init(void);
void *kmalloc(size_t size);
void *kcalloc(size_t count, size_t size);
void *krealloc(void *ptr, size_t new_size);
void kfree(void *ptr);
size_t heap_used(void);
size_t heap_available(void);

#endif
