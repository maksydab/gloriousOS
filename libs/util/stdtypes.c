
#include "stdtypes.h"
#include "../mm/heap.h"

static size_t str_len(const char *s) {
    size_t len = 0;

    if (!s)
        return 0;

    while (s[len])
        len++;

    return len;
}

static void str_copy(char *dst, const char *src, size_t n) {
    for (size_t i = 0; i < n; i++)
        dst[i] = src[i];
}

static void str_move(char *dst, const char *src, size_t n) {
    if (dst < src) {
        for (size_t i = 0; i < n; i++)
            dst[i] = src[i];
    } else if (dst > src) {
        for (size_t i = n; i > 0; i--)
            dst[i - 1] = src[i - 1];
    }
}

bool string_init(String *s, size_t initial_cap) {
    if (!s)
        return false;

    if (initial_cap < 1)
        initial_cap = 16;

    s->data = kmalloc(initial_cap);

    if (!s->data) {
        s->len = 0;
        s->cap = 0;
        return false;
    }

    s->data[0] = '\0';
    s->len = 0;
    s->cap = initial_cap;

    return true;
}

bool string_init_copy(String *s, const char *str) {
    if (!s || !str)
        return false;

    size_t len = str_len(str);

    if (len == (size_t)-1)
        return false;

    if (!string_init(s, len + 1))
        return false;

    str_copy(s->data, str, len + 1);
    s->len = len;

    return true;
}

bool string_reserve(String *s, size_t capacity) {
    if (!s)
        return false;

    if (capacity <= s->cap)
        return true;

    if (capacity > (size_t)-1 / 2)
        return false;

    size_t new_cap = s->cap ? s->cap : 16;

    while (new_cap < capacity) {
        if (new_cap > (size_t)-1 / 2) {
            new_cap = capacity;
            break;
        }

        new_cap *= 2;
    }

    char *new_data = krealloc(s->data, new_cap);

    if (!new_data)
        return false;

    s->data = new_data;
    s->cap = new_cap;

    return true;
}


bool string_set(String *s, const char *str) {
    if (!s || !str)
        return false;

    size_t len = str_len(str);

    if (len == (size_t)-1)
        return false;

    if (len == (size_t)-1 - 1)
        return false;
    bool internal = false;
    size_t offset = 0;

    if (s->data && s->cap) {
        uintptr_t base = (uintptr_t)s->data;
        uintptr_t source = (uintptr_t)str;

        if (source >= base && source - base < s->cap) {
            internal = true;
            offset = source - base;
        }
    }
    if (!string_reserve(s, len + 1))
        return false;
    if (internal)
        str = s->data + offset;
    str_move(s->data, str, len + 1);
    s->len = len;
    return true;
}

bool string_append(String *s, const char *str) {
    if (!s || !str)
        return false;
    size_t len = str_len(str);
    if (len > (size_t)-1 - s->len - 1)
        return false;
    bool internal = false;
    size_t offset = 0;
    if (s->data && s->cap) {
        uintptr_t base = (uintptr_t)s->data;
        uintptr_t source = (uintptr_t)str;

        if (source >= base && source - base < s->cap) {
            internal = true;
            offset = source - base;
        }
    }
    size_t old_len = s->len;
    if (!string_reserve(s, old_len + len + 1))
        return false;
    if (internal)
        str = s->data + offset;
    str_move(s->data + old_len, str, len + 1);
    s->len += len;
    return true;
}

bool string_append_char(String *s, char c) {
    if (!s || s->len == (size_t)-1)
        return false;

    if (!string_reserve(s, s->len + 2))
        return false;

    s->data[s->len++] = c;
    s->data[s->len] = '\0';

    return true;
}

void string_truncate(String *s, size_t new_len) {
    if (!s || !s->data || new_len > s->len)
        return;

    s->len = new_len;
    s->data[new_len] = '\0';
}

void string_clear(String *s) {
    if (!s)
        return;

    s->len = 0;

    if (s->data)
        s->data[0] = '\0';
}

void string_free(String *s) {
    if (!s)
        return;

    if (s->cap)
        kfree(s->data);

    s->data = NULL;
    s->len = 0;
    s->cap = 0;
}
void string_rem(String *s, size_t a, size_t b) {
    if (!s || !s->data || a >= s->len || a > b)
        return;
    if (b >= s->len)
        b = s->len - 1;

    size_t remove_len = b - a + 1;
    str_move(
        s->data + a,
        s->data + b + 1,
        s->len - b
    );

    s->len -= remove_len;
}
size_t string_length(const String *s) {
    return s ? s->len : 0;
}

const char *string_cstr(const String *s) {
    return (s && s->data) ? s->data : "";
}

int string_compare(const String *a, const String *b) {
    if (!a || !b)
        return a ? 1 : (b ? -1 : 0);

    if (a == b)
        return 0;

    const char *s1 = string_cstr(a);
    const char *s2 = string_cstr(b);

    while (*s1 && *s1 == *s2) {
        s1++;
        s2++;
    }

    return (unsigned char)*s1 - (unsigned char)*s2;
}
int string_compare_cstr(const String *a, const char *b) {
    if (!a || !b)
        return a ? 1 : (b ? -1 : 0);

    const char *s1 = string_cstr(a);

    while (*s1 && *s1 == *b) {
        s1++;
        b++;
    }

    return (unsigned char)*s1 - (unsigned char)*b;
}