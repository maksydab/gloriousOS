
#ifndef GLORIOUS_DISPLAY_H
#define GLORIOUS_DISPLAY_H
#include "../libs/util/stdtypes.h"
void display_init(void);

void clear(unsigned char attr);

void print(const char *n, const char *msg,
           unsigned char stat, unsigned char msgc);
void set_cursor(int x, int y);
int get_cursor_x(void);
int get_cursor_y(void);
void print_string(const String *s, unsigned char attr);
#endif
