
#ifndef GLORIOUS_STD_TYPES_H
#define GLORIOUS_STD_TYPES_H

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
/*
this file is used to implement most types used in this kernel, the standard ones and the custom built ones
for now we have strings, i know ton of them, but i think more will come with time
*/
typedef struct {
    char *data;
    size_t len;
    size_t cap;
} String;

#define STRING(str) ((String){ \
    .data = (char *)(str), \
    .len = sizeof(str) - 1, \
    .cap = 0 \
})

bool string_init(String *s, size_t initial_cap);
bool string_init_copy(String *s, const char *str);
bool string_set(String *s, const char *str);
bool string_append(String *s, const char *str);
bool string_append_char(String *s, char c);
void string_truncate(String *s, size_t new_len);
void string_clear(String *s);
bool string_reserve(String *s, size_t capacity);
void string_free(String *s);
size_t string_length(const String *s);
const char *string_cstr(const String *s);
void string_rem(String *s, size_t a, size_t b);
int string_compare(const String *a, const String *b);
int string_compare_cstr(const String *a, const char *b);
#endif
