#include "log.h"

#define COM1 0x3F8
static inline uint8_t  inb(uint16_t p){uint8_t v;__asm__ volatile("inb %1,%0":"=a"(v):"Nd"(p));return v;}
static inline void     outb(uint16_t p,uint8_t v){__asm__ volatile("outb %0,%1"::"a"(v),"Nd"(p));}

void log_init(void){
    outb(COM1+1,0x00); outb(COM1+3,0x80);
    outb(COM1+0,0x03); outb(COM1+1,0x00);
    outb(COM1+3,0x03); outb(COM1+2,0xC7);
    outb(COM1+4,0x03);
}
static void log_putc(char c){
    for(volatile uint32_t i=0;i<10000;i++) if(inb(COM1+5)&0x20) break;
    outb(COM1,(uint8_t)c);
}
void log_str(const char *s){
    while(*s){ if(*s=='\n') log_putc('\r'); log_putc(*s++); }
}
void log_hex(const char *label, uint32_t v){
    static const char h[]="0123456789ABCDEF";
    log_str(label); log_str("=0x");
    for(int i=28;i>=0;i-=4) log_putc(h[(v>>i)&0xF]);
    log_str("\n");
}
void log_dec(const char *label, uint32_t v){
    char b[11]; int i=10; b[i--]=0;
    if(!v) b[i--]='0';
    while(v){ b[i--]='0'+(v%10); v/=10; }
    log_str(label); log_str("="); log_str(&b[i+1]); log_str("\n");
}