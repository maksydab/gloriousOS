
#ifndef GLORIOUS_PIC_H
#define GLORIOUS_PIC_H

#include <stdint.h>

#define PIC1_CMD  0x20U
#define PIC1_DATA 0x21U
#define PIC2_CMD  0xA0U
#define PIC2_DATA 0xA1U
#define PIC_EOI   0x20U

void pic_remap(uint8_t offset1, uint8_t offset2);
void pic_send_eoi(uint8_t irq);
void pic_set_mask(uint8_t irq);
void pic_clear_mask(uint8_t irq);

#endif
