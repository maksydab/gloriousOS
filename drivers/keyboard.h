
#ifndef GLORIOUS_KEYBOARD_H
#define GLORIOUS_KEYBOARD_H

#include <stdbool.h>
#include <stdint.h>

typedef enum {
    KEY_NONE = 0,

    KEY_CHAR,

    KEY_BACKSPACE,
    KEY_DELETE,

    KEY_LEFT,
    KEY_RIGHT,
    KEY_UP,
    KEY_DOWN,

    KEY_HOME,
    KEY_END,

    KEY_ENTER,
    KEY_ESCAPE,
    KEY_TAB,

} KeyCode;

typedef struct {
    KeyCode code;
    char ch;
} KeyEvent;

void keyboard_init(void);
void keyboard_irq_handler(void);

bool kbd_pop_event(KeyEvent *event);

#endif
