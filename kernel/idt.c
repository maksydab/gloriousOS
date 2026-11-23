#include <stdint.h>

struct idt_entry {
  uint16_t offset_low;
  uint16_t selector;
  uint8_t zero;
  uint8_t type_attr;
  uint16_t offset_high;
} __attribute__((packed));

struct idt_ptr {
  uint16_t limit;
  uint32_t base;
} __attribute__((packed));

extern void idt_flush(uint32_t ptr);
extern void irq0_stub(void);
extern void irq1_stub(void);

static struct idt_entry idt[256];
static struct idt_ptr idtp;

static void idt_set_gate(uint8_t num, uint32_t base, uint16_t sel,
                         uint8_t flags) {
  idt[num].offset_low = (uint16_t)(base & 0xFFFFU);
  idt[num].selector = sel;
  idt[num].zero = 0;
  idt[num].type_attr = flags;
  idt[num].offset_high = (uint16_t)((base >> 16) & 0xFFFFU);
}

void idt_init(void) {
  idtp.limit = (uint16_t)(sizeof(idt) - 1U);
  idtp.base = (uint32_t)&idt;

  for (int i = 0; i < 256; i++) {
    idt_set_gate((uint8_t)i, 0, 0x08, 0x8E);
  }

  // 0x08 = kenrel segment code sleector; 0x8E = present, ring 0, 32-bit
  // interrupt gate
  idt_set_gate(32, (uint32_t)irq0_stub, 0x08, 0x8E);
  idt_set_gate(33, (uint32_t)irq1_stub, 0x08, 0x8E);

  idt_flush((uint32_t)&idtp);
}