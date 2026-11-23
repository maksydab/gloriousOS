#include "io.h"
#include "pic.h"
#include <stdbool.h>
#include <stdint.h>

extern void pic_send_eoi(uint8_t irq);
extern void print(const char *n, const char *msg, unsigned char stat,
                  unsigned char msgc);

#define KBD_DATA_PORT 0x60
#define KBD_STATUS_PORT 0x64
#define KBD_BUF_SIZE 128

static volatile char kbd_buf[KBD_BUF_SIZE];
static volatile uint32_t kbd_head = 0;
static volatile uint32_t kbd_tail = 0;
static bool shift_held = false;

// US QWERTY SUBSET
static const char scancode_normal[128] = {
    0,   27,  '1',  '2',  '3',  '4', '5', '6',  '7', '8', '9', '0',
    '-', '=', '\b', '\t', 'q',  'w', 'e', 'r',  't', 'y', 'u', 'i',
    'o', 'p', '[',  ']',  '\n', 0,   'a', 's',  'd', 'f', 'g', 'h',
    'j', 'k', 'l',  ';',  '\'', '`', 0,   '\\', 'z', 'x', 'c', 'v',
    'b', 'n', 'm',  ',',  '.',  '/', 0,   '*',  0,   ' '};

static const char scancode_shift[128] = {
    0,   27,  '!',  '@',  '#',  '$', '%', '^', '&', '*', '(', ')',
    '_', '+', '\b', '\t', 'Q',  'W', 'E', 'R', 'T', 'Y', 'U', 'I',
    'O', 'P', '{',  '}',  '\n', 0,   'A', 'S', 'D', 'F', 'G', 'H',
    'J', 'K', 'L',  ':',  '"',  '~', 0,   '|', 'Z', 'X', 'C', 'V',
    'B', 'N', 'M',  '<',  '>',  '?', 0,   '*', 0,   ' '};

static void kbd_push_char(char c) {
  uint32_t next = (kbd_head + 1U) % KBD_BUF_SIZE;
  if (next != kbd_tail) {
    kbd_buf[kbd_head] = c;
    kbd_head = next;
  }
}

bool kbd_pop_char(char *out_c) {
  if (kbd_head == kbd_tail) {
    return false;
  }
  *out_c = kbd_buf[kbd_tail];
  kbd_tail = (kbd_tail + 1U) % KBD_BUF_SIZE;
  return true;
}

void keyboard_irq_handler(void) {
  uint8_t status = inb(KBD_STATUS_PORT);
  if (status & 0x01) {
    uint8_t sc = inb(KBD_DATA_PORT);

    if (sc == 0x2A || sc == 0x36) {
      shift_held = true;
    } else if (sc == (0x2A | 0x80) || sc == (0x36 | 0x80)) {
      shift_held = false;
    } else if (!(sc & 0x80) && sc < 128) {
      char c = shift_held ? scancode_shift[sc] : scancode_normal[sc];
      if (c != 0) {
        kbd_push_char(c);
      }
    }
  }
  pic_send_eoi(1);
}

void keyboard_init(void) {
  outb(PIC1_DATA, (uint8_t)~0x03U);
  outb(PIC2_DATA, 0xFFU);
}