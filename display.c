#define VGA_WIDTH 80
#define VGA_HEIGHT 25

volatile unsigned short *vga = (volatile unsigned short *)0xB8000;
int cursor = 0;

static void putc_vga(char ch, unsigned char attr) {
  if (ch == '\n') {
    cursor = (cursor / VGA_WIDTH + 1) * VGA_WIDTH;
    return;
  }
  if (ch == '\r') {
    cursor = (cursor / VGA_WIDTH) * VGA_WIDTH;
    return;
  }

  vga[cursor] = ((unsigned short)attr << 8) | (unsigned char)ch;
  cursor++;
}

static void puts_colored(const char *s, unsigned char attr) {
  if (!s)
    return;
  for (int i = 0; s[i] != '\0'; i++) {
    putc_vga(s[i], attr);
  }
}

void print(const char *n, const char *msg, unsigned char stat,
           unsigned char msgc) {
  puts_colored(n, stat);
  puts_colored(msg, msgc);
}