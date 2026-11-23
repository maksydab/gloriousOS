#include "display.c"
#include "keyboard.c"

extern void idt_init(void);
extern void pic_remap(uint8_t offset1, uint8_t offset2);
extern void pit_init(uint32_t freq_hz);
extern uint32_t get_ticks(void);

void kmain(void) {
  vga = (volatile unsigned short *)0xB8000;
  print("[Boot] ", "Booting sequence started", 0x0B, 0x0F);
  print("\n[Boot] ", "initializing IDT, PIT and PS/2 keyboard\n", 0x0B, 0x0F);

  idt_init();
  pic_remap(0x20, 0x28); // IRQ0 -> 32  IRQ1 -> 33
  pit_init(1000);        // 1ms system tick
  keyboard_init();
  sti();

  uint32_t last_hb = get_ticks();
  char ch;
  char one_char[2] = {0, 0};

  for (;;) {
    // keyboard inputs
    while (kbd_pop_char(&ch)) {
      one_char[0] = ch;
      print("", one_char, 0x0F, 0x0F);
    }

    // periodic refreshement
    if ((uint32_t)(get_ticks() - last_hb) >= 5000U) {
      last_hb = get_ticks();
      print("\n[hi] ", "refreshement\n", 0x0F, 0x0A);
    }

    __asm__ volatile("hlt"); // wait until next interrupt
  }
}