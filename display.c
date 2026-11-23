volatile unsigned short *vga;
int cursor = 0;

void print(const char *n, const char *msg, unsigned char stat,
           unsigned char msgc) {
  unsigned char attr = stat;
  for (int i = 0; n[i] != '\0'; i++) {
    vga[cursor] = ((unsigned short)attr << 8) | (unsigned char)n[i];
    cursor++;
  }
  attr = msgc;
  for (int i = 0; msg[i] != '\0'; i++) {
    vga[cursor] = ((unsigned short)attr << 8) | (unsigned char)msg[i];
    cursor++;
  }
}