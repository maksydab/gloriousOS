
#include "heap.h"
#include <stdint.h>

#define HEAP_SIZE (1024 * 1024)
#define HEAP_ALIGN 16
#define ALIGN_UP(x) (((x) + HEAP_ALIGN - 1) & ~(HEAP_ALIGN - 1))

typedef struct Block {
    size_t size;
    bool free;
    struct Block *next;
} Block;

#define BLOCK_HEADER_SIZE ALIGN_UP(sizeof(Block))

static uint8_t heap[HEAP_SIZE] __attribute__((aligned(16)));

static Block *first_block = NULL;
static bool initialized = false;

static void *block_data(Block *block) {
    return (void *)((uint8_t *)block + BLOCK_HEADER_SIZE);
}

static Block *data_block(void *ptr) {
    return (Block *)((uint8_t *)ptr - BLOCK_HEADER_SIZE);
}

static void memory_copy(void *dst, const void *src, size_t n) {
    uint8_t *d = dst;
    const uint8_t *s = src;

    for (size_t i = 0; i < n; i++)
        d[i] = s[i];
}

static void memory_zero(void *ptr, size_t n) {
    uint8_t *p = ptr;

    for (size_t i = 0; i < n; i++)
        p[i] = 0;
}

bool heap_init(void) {
    if (initialized)
        return true;

    first_block = (Block *)heap;

    first_block->size = HEAP_SIZE - BLOCK_HEADER_SIZE;
    first_block->free = true;
    first_block->next = NULL;

    initialized = true;
    return true;
}

static void split_block(Block *block, size_t size) {
    if (block->size < size + BLOCK_HEADER_SIZE + HEAP_ALIGN)
        return;

    Block *next = (Block *)(
        (uint8_t *)block_data(block) + size
    );

    next->size = block->size - size - BLOCK_HEADER_SIZE;
    next->free = true;
    next->next = block->next;

    block->size = size;
    block->next = next;
}

void *kmalloc(size_t size) {
    if (!initialized && !heap_init())
        return NULL;

    if (size == 0)
        return NULL;

    if (size > SIZE_MAX - (HEAP_ALIGN - 1))
        return NULL;

    size = ALIGN_UP(size);

    Block *current = first_block;

    while (current) {
        if (current->free && current->size >= size) {
            split_block(current, size);
            current->free = false;

            return block_data(current);
        }

        current = current->next;
    }

    return NULL;
}

void kfree(void *ptr) {
    if (!ptr || !initialized)
        return;

    Block *current = first_block;

    while (current) {
        if (block_data(current) == ptr)
            break;

        current = current->next;
    }

    if (!current || current->free)
        return;

    current->free = true;

    current = first_block;

    while (current && current->next) {
        Block *next = current->next;

        if (current->free && next->free) {
            current->size += BLOCK_HEADER_SIZE + next->size;
            current->next = next->next;
        } else {
            current = next;
        }
    }
}

void *kcalloc(size_t count, size_t size) {
    if (size != 0 && count > SIZE_MAX / size)
        return NULL;

    size_t total = count * size;

    void *ptr = kmalloc(total);

    if (ptr)
        memory_zero(ptr, total);

    return ptr;
}

void *krealloc(void *ptr, size_t new_size) {
    if (!ptr)
        return kmalloc(new_size);

    if (new_size == 0) {
        kfree(ptr);
        return NULL;
    }

    if (!initialized)
        return NULL;

    Block *current = first_block;

    while (current && block_data(current) != ptr)
        current = current->next;

    if (!current || current->free)
        return NULL;

    if (new_size <= current->size)
        return ptr;

    void *new_ptr = kmalloc(new_size);

    if (!new_ptr)
        return NULL;

    memory_copy(new_ptr, ptr, current->size);
    kfree(ptr);

    return new_ptr;
}

size_t heap_used(void) {
    size_t used = 0;

    for (Block *b = first_block; b; b = b->next) {
        if (!b->free)
            used += b->size;
    }

    return used;
}

size_t heap_available(void) {
    size_t available = 0;

    for (Block *b = first_block; b; b = b->next) {
        if (b->free)
            available += b->size;
    }

    return available;
}
