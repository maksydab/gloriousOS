#include "pit.h"
#include "pic.h"
#include "../libs/util/io.h"
#define PIT_CHAN0 0x40U
#define PIT_CMD 0x43U
#define PIT_BASE_HZ 1193182U

static volatile uint32_t ticks = 0;

void pit_init(uint32_t freq_hz) {
  if (freq_hz == 0U) {
    freq_hz = 100U;
  }
  uint32_t divisor = PIT_BASE_HZ / freq_hz;
  if (divisor == 0U) {
    divisor = 1U;
  } else if (divisor > 0xFFFFU) {
    divisor = 0xFFFFU;
  }

  // Channel 0, access lo/hi byte, mode 2 (rate generator), binary
  outb(PIT_CMD, 0x34U);
  outb(PIT_CHAN0, (uint8_t)(divisor & 0xFFU));
  outb(PIT_CHAN0, (uint8_t)((divisor >> 8) & 0xFFU));
}

void pit_irq_handler(void) {
  ticks++;
  pic_send_eoi(0);
}

uint32_t get_ticks(void) { return ticks; }