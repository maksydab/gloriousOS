#include <stdint.h>
#include "../libs/util/io.h"
#define VGA_WIDTH 80
#define VGA_HEIGHT 25
#define VGA_SIZE (VGA_WIDTH * VGA_HEIGHT)

volatile unsigned short *vga = (volatile unsigned short *)0xB8000;
int cursor = 0;


static void update_hw_cursor(void) {
  uint16_t pos = (uint16_t)cursor;

  outb(0x3D4, 0x0F);
  outb(0x3D5, (uint8_t)(pos & 0xFF));

  outb(0x3D4, 0x0E);
  outb(0x3D5, (uint8_t)((pos >> 8) & 0xFF));
}

static void scroll_vga(unsigned char blank_attr) {
  if (cursor < VGA_SIZE) {
    return;
  }
  for (int i = 0; i < (VGA_HEIGHT - 1) * VGA_WIDTH; i++) {
    vga[i] = vga[i + VGA_WIDTH];
  }
  unsigned short blank = ((unsigned short)blank_attr << 8) | (unsigned char)' ';
  for (int i = (VGA_HEIGHT - 1) * VGA_WIDTH; i < VGA_SIZE; i++) {
    vga[i] = blank;
  }
  cursor = (VGA_HEIGHT - 1) * VGA_WIDTH;
}

static void putc_vga(char ch, unsigned char attr) {
  if (ch == '\n') {
    cursor = (cursor / VGA_WIDTH + 1) * VGA_WIDTH;
    scroll_vga(attr);
    return;
  }
  if (ch == '\r') {
    cursor = (cursor / VGA_WIDTH) * VGA_WIDTH;
    return;
  }

  vga[cursor] = ((unsigned short)attr << 8) | (unsigned char)ch;
  cursor++;
  scroll_vga(attr);
}

static void puts_colored(const char *s, unsigned char attr) {
  if (!s) {
    return;
  }
  for (int i = 0; s[i] != '\0'; i++) {
    putc_vga(s[i], attr);
  }
}

void print(const char *n, const char *msg, unsigned char stat,
           unsigned char msgc) {
  puts_colored(n, stat);
  puts_colored(msg, msgc);
  update_hw_cursor();
}
void display_init(void) {
    vga = (volatile unsigned short *)0xB8000;
    cursor = 0;
}