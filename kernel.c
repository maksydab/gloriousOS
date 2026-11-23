#include "display.c"
#include "keyboard.c"
#include "libs/stdtypes.h"
void kmain(void) {
  vga = (volatile unsigned short *)0xB8000;
  const char *msg = "Booting sequence started";
  print("[Boot] ", msg, 0x0B, 0x0F);
}