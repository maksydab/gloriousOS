
#ifndef GLORIOUS_KEYBOARD_H
#define GLORIOUS_KEYBOARD_H

#include <stdbool.h>

void keyboard_init(void);
void keyboard_irq_handler(void);
bool kbd_pop_char(char *out_c);

#endif
