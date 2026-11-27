
#include "keyboard.h"
#include "../kernel/pic.h"
#include "../libs/util/io.h"

#define KBD_DATA_PORT 0x60
#define KBD_STATUS_PORT 0x64
#define KBD_BUF_SIZE 128

static volatile KeyEvent kbd_buf[KBD_BUF_SIZE];

static volatile uint32_t kbd_head = 0;
static volatile uint32_t kbd_tail = 0;

static bool shift_held = false;
static bool extended = false;

// US QWERTY
static const char scancode_normal[128] = {
    0, 27, '1', '2', '3', '4', '5', '6',
    '7', '8', '9', '0', '-', '=', 0, 9,
    'q', 'w', 'e', 'r', 't', 'y', 'u', 'i',
    'o', 'p', '[', ']', 0, 0, 'a', 's',
    'd', 'f', 'g', 'h', 'j', 'k', 'l', ';',
    '\'', '`', 0, '\\', 'z', 'x', 'c', 'v',
    'b', 'n', 'm', ',', '.', '/', 0, '*',
    0, ' '
};

static const char scancode_shift[128] = {
    0, 27, '!', '@', '#', '$', '%', '^',
    '&', '*', '(', ')', '_', '+', 0, 9,
    'Q', 'W', 'E', 'R', 'T', 'Y', 'U', 'I',
    'O', 'P', '{', '}', 0, 0, 'A', 'S',
    'D', 'F', 'G', 'H', 'J', 'K', 'L', ':',
    '"', '~', 0, '|', 'Z', 'X', 'C', 'V',
    'B', 'N', 'M', '<', '>', '?', 0, '*',
    0, ' '
};

static void kbd_push_event(KeyCode code, char ch) {
    uint32_t next = (kbd_head + 1U) % KBD_BUF_SIZE;

    if (next == kbd_tail)
        return;

    kbd_buf[kbd_head].code = code;
    kbd_buf[kbd_head].ch = ch;

    kbd_head = next;
}

bool kbd_pop_event(KeyEvent *event) {
    if (!event || kbd_head == kbd_tail)
        return false;

    event->code = kbd_buf[kbd_tail].code;
    event->ch = kbd_buf[kbd_tail].ch;

    kbd_tail = (kbd_tail + 1U) % KBD_BUF_SIZE;

    return true;
}

static void handle_extended(uint8_t sc) {
    switch (sc) {
        case 0x4B: kbd_push_event(KEY_LEFT, 0);  break;
        case 0x4D: kbd_push_event(KEY_RIGHT, 0); break;
        case 0x48: kbd_push_event(KEY_UP, 0);    break;
        case 0x50: kbd_push_event(KEY_DOWN, 0);  break;
        case 0x53: kbd_push_event(KEY_DELETE, 0); break;
        case 0x47: kbd_push_event(KEY_HOME, 0);  break;
        case 0x4F: kbd_push_event(KEY_END, 0);   break;
        default: break;
    }
}

void keyboard_irq_handler(void) {
    uint8_t status = inb(KBD_STATUS_PORT);

    if (!(status & 0x01))
        return;

    uint8_t sc = inb(KBD_DATA_PORT);
    if (sc == 0xE0) {
        extended = true;
        return;
    }
    if (sc & 0x80) {
        uint8_t released = sc & 0x7F;

        if (!extended) {
            if (released == 0x2A || released == 0x36)
                shift_held = false;
        }

        extended = false;
        return;
    }

    if (extended) {
        handle_extended(sc);
        extended = false;
        return;
    }
    if (sc == 0x2A || sc == 0x36) {
        shift_held = true;
        return;
    }
    switch (sc) {
        case 0x0E:
            kbd_push_event(KEY_BACKSPACE, 0);
            return;

        case 0x1C:
            kbd_push_event(KEY_ENTER, 0);
            return;

        case 0x01:
            kbd_push_event(KEY_ESCAPE, 0);
            return;

        case 0x0F:
            kbd_push_event(KEY_TAB, 0);
            return;
    }
    if (sc < 128) {
        char c = shift_held
            ? scancode_shift[sc]
            : scancode_normal[sc];

        if (c)
            kbd_push_event(KEY_CHAR, c);
    }
}

void keyboard_init(void) {
    kbd_head = 0;
    kbd_tail = 0;
    shift_held = false;
    extended = false;

    pic_clear_mask(1);
}
