#ifndef GLORIOUS_LOG_H
#define GLORIOUS_LOG_H
#include <stdint.h>

void log_init(void);
void log_str(const char *s);
void log_hex(const char *label, uint32_t v);
void log_dec(const char *label, uint32_t v);

#endif