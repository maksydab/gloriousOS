#include "pic.h"
#include "io.h"

void pic_remap(uint8_t offset1, uint8_t offset2) {
  uint8_t mask1 = inb(PIC1_DATA);
  uint8_t mask2 = inb(PIC2_DATA);

  outb(PIC1_CMD, 0x11U);
  io_wait();
  outb(PIC2_CMD, 0x11U);
  io_wait();

  outb(PIC1_DATA, offset1);
  io_wait();
  outb(PIC2_DATA, offset2);
  io_wait();

  outb(PIC1_DATA, 0x04U);
  io_wait();
  outb(PIC2_DATA, 0x02U);
  io_wait();

  outb(PIC1_DATA, 0x01U);
  io_wait();
  outb(PIC2_DATA, 0x01U);
  io_wait();

  outb(PIC1_DATA, mask1);
  outb(PIC2_DATA, mask2);
}

void pic_send_eoi(uint8_t irq) {
  if (irq >= 8U) {
    outb(PIC2_CMD, PIC_EOI);
  }
  outb(PIC1_CMD, PIC_EOI);
}

void pic_set_mask(uint8_t irq) {
  uint16_t port = (irq < 8U) ? PIC1_DATA : PIC2_DATA;
  uint8_t bit = (uint8_t)(1U << (irq % 8U));
  outb(port, (uint8_t)(inb(port) | bit));
}

void pic_clear_mask(uint8_t irq) {
  uint16_t port = (irq < 8U) ? PIC1_DATA : PIC2_DATA;
  uint8_t bit = (uint8_t)(1U << (irq % 8U));
  outb(port, (uint8_t)(inb(port) & (uint8_t)~bit));
}