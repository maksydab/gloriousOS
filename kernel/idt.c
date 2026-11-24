#include <stdint.h>
#include <stdbool.h>
#include "idt.h"
#include "pic.h"
#include "pit.h"
#include "../drivers/keyboard.h"
#include "../libs/util/log.h"
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
// thanks to chatgpt for helping to debug

/* Matches the frame built by isr_common in idt.s, low -> high:
 *   [gs][fs][es][ds]
 *   [edi][esi][ebp][esp_dummy][ebx][edx][ecx][eax]
 *   [vector][err][eip][cs][eflags]
 */
typedef struct {
    uint32_t gs, fs, es, ds;
    uint32_t edi, esi, ebp, esp_dummy, ebx, edx, ecx, eax;
    uint32_t vector;
    uint32_t err;
    uint32_t eip, cs, eflags;
} regs_t;

extern void idt_flush(uint32_t ptr);
extern void irq0_stub(void);
extern void irq1_stub(void);
extern void (*exc_stub_table[32])(void);

static struct idt_entry idt[256];
static struct idt_ptr idtp;

static void idt_set_gate(uint8_t num, uint32_t base, uint16_t sel, uint8_t flags) {
  idt[num].offset_low  = (uint16_t)(base & 0xFFFFU);
  idt[num].selector    = sel;
  idt[num].zero        = 0;
  idt[num].type_attr   = flags;
  idt[num].offset_high = (uint16_t)((base >> 16) & 0xFFFFU);
}

/* ---------- serial helpers used only by exception path ---------- */
#define COM1 0x3F8
static inline void outb_e(uint16_t p, uint8_t v){ __asm__ volatile("outb %0,%1"::"a"(v),"Nd"(p)); }
static inline uint8_t inb_e(uint16_t p){ uint8_t v; __asm__ volatile("inb %1,%0":"=a"(v):"Nd"(p)); return v; }

static void ser_init_e(void) {
    outb_e(COM1+1,0x00); outb_e(COM1+3,0x80);
    outb_e(COM1+0,0x03); outb_e(COM1+1,0x00);
    outb_e(COM1+3,0x03); outb_e(COM1+2,0xC7);
    outb_e(COM1+4,0x03);
}
static void ser_c(char c){ for(uint32_t i=0;i<1000;i++) if(inb_e(COM1+5)&0x20) break; outb_e(COM1,(uint8_t)c); }
static void ser_s(const char*s){ while(*s){ if(*s=='\n') ser_c('\r'); ser_c(*s++); } }
static void ser_x(uint32_t v){
    static const char h[]="0123456789ABCDEF";
    ser_s("0x");
    for(int i=28;i>=0;i-=4) ser_c(h[(v>>i)&0xF]);
}

/* ---------- called for EVERY interrupt ---------- */
void exception_dispatch(regs_t *r) {
    if (r->vector < 32) {
        /* CPU exception — halt loudly */
        ser_init_e();
        ser_s("\n!!! EXCEPTION vector=");
        ser_x(r->vector);
        ser_s(" err=");    ser_x(r->err);
        ser_s(" eip=");    ser_x(r->eip);
        ser_s(" cs=");     ser_x(r->cs);
        ser_s(" eflags="); ser_x(r->eflags);
        ser_s("\n");
        for(;;) __asm__ volatile("cli; hlt");
    }
    if (r->vector == 32) {
        pit_irq_handler();
        pic_send_eoi(0);
        return;
    }
    if (r->vector == 33) {
        keyboard_irq_handler();
        pic_send_eoi(1);
        return;
    }
    /* Unknown IRQ — send EOI to keep going. Guard against bogus vectors. */
    if (r->vector >= 32 && r->vector < 48) {
        pic_send_eoi((uint8_t)(r->vector - 32));
    }
}

void idt_init(void) {
    log_str("[idt] enter\n");

    log_str("[idt] writing idtp\n");
    idtp.limit = (uint16_t)(sizeof(idt) - 1U);
    idtp.base  = (uint32_t)&idt;
    log_str("[idt] idtp written\n");

    log_str("[idt] clearing gates 0..255\n");
    for (int i = 0; i < 256; i++) {
        idt_set_gate((uint8_t)i, 0, 0x10, 0x00);
    }
    log_str("[idt] gates cleared\n");

    log_str("[idt] installing 32 exception gates\n");
    for (int i = 0; i < 32; i++) {
        log_str("[idt]  exc ");
        log_dec("", (uint32_t)i);
        idt_set_gate((uint8_t)i, (uint32_t)exc_stub_table[i], 0x10, 0x8E);
    }
    log_str("[idt] exceptions installed\n");

    log_str("[idt] installing irq gates\n");
    idt_set_gate(32, (uint32_t)irq0_stub, 0x10, 0x8E);
    idt_set_gate(33, (uint32_t)irq1_stub, 0x10, 0x8E);
    log_str("[idt] irqs installed\n");

    log_str("[idt] calling lidt\n");
    idt_flush((uint32_t)&idtp);
    log_str("[idt] lidt returned\n");
}