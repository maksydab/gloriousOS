
#ifndef GLORIOUS_PIT_H
#define GLORIOUS_PIT_H

#include <stdint.h>

void pit_init(uint32_t freq_hz);
void pit_irq_handler(void);
uint32_t get_ticks(void);

#endif
