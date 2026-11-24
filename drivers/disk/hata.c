#include "ata.h"
#include "../../libs/util/log.h"
// io
static inline uint8_t  inb (uint16_t p){uint8_t v;__asm__ volatile("inb %1,%0":"=a"(v):"Nd"(p));return v;}
static inline void     outb(uint16_t p,uint8_t v){__asm__ volatile("outb %0,%1"::"a"(v),"Nd"(p));}
static inline void     insw(uint16_t p,void*a,uint32_t n){__asm__ volatile("rep insw":"+D"(a),"+c"(n):"d"(p):"memory");}
static inline void     outsw(uint16_t p,const void*a,uint32_t n){__asm__ volatile("rep outsw":"+S"(a),"+c"(n):"d"(p):"memory");}

// serial debugggggg
#define COM1 0x3F8
static void ser_init(void){
    outb(COM1+1,0x00); outb(COM1+3,0x80);
    outb(COM1+0,0x03); outb(COM1+1,0x00);
    outb(COM1+3,0x03); outb(COM1+2,0xC7);
    outb(COM1+4,0x03);
}
static void ser_putc(char c){
    for(volatile uint32_t i=0;i<100000;i++) if(inb(COM1+5)&0x20) break;
    outb(COM1,(uint8_t)c);
}
static void ser_str(const char*s){while(*s){if(*s=='\n')ser_putc('\r');ser_putc(*s++);}}
static void ser_hex(const char*label,uint32_t v){
    static const char h[]="0123456789ABCDEF";
    ser_str(label); ser_str("=0x");
    for(int i=28;i>=0;i-=4) ser_putc(h[(v>>i)&0xF]);
    ser_putc('\n');
}
#define T(m)        ser_str("[ata] " m "\n")
#define TV(m,v)     ser_hex("[ata] " m, (uint32_t)(v))

// ata ports for prim bus
#define P_DATA      0x1F0
#define P_SECCOUNT  0x1F2
#define P_LBA0      0x1F3
#define P_LBA1      0x1F4
#define P_LBA2      0x1F5
#define P_DRIVE     0x1F6
#define P_STATUS    0x1F7
#define P_CMD       0x1F7
#define P_CTRL      0x3F6

#define S_ERR   0x01
#define S_DRQ   0x08
#define S_DF    0x20
#define S_DRDY  0x40
#define S_BSY   0x80

#define CMD_READ28   0x20
#define CMD_WRITE28  0x30
#define CMD_FLUSH    0xE7
#define CMD_IDENTIFY 0xEC

#define TIMEOUT 200000u

static uint32_t g_sectors = 0;
static bool     g_ready   = false;

static void delay_400(void){ for(int i=0;i<4;i++)(void)inb(P_CTRL); }

// returns current status until bsy is clear
static uint8_t wait_not_bsy(void){
    for(uint32_t i=0;i<TIMEOUT;i++){
        uint8_t st = inb(P_STATUS);
        if(st == 0xFF) return 0xFF;
        if(!(st & S_BSY)) return st;
    }
    return 0xFF;
}

//returns status once with drq=1 or bsy=0
static uint8_t wait_drq(void){
    for(uint32_t i=0;i<TIMEOUT;i++){
        uint8_t st = inb(P_STATUS);
        if(st & (S_ERR|S_DF)) return st;
        if(!(st & S_BSY) && (st & S_DRQ)) return st;
    }
    return 0xFF;
}

// publis methods

bool ata_init(void){
    g_ready   = false;
    g_sectors = 0;

    T("init: enter");

    outb(P_CTRL, 0x02);
    T("init: IRQs masked");

    TV("init: pre-select status", inb(P_STATUS));

    outb(P_DRIVE, 0xA0);
    delay_400();
    TV("init: post-select status", inb(P_STATUS));

    uint8_t st = wait_not_bsy();
    TV("init: wait_not_bsy -> st", st);
    if(st == 0xFF){ T("init: FAIL: drive stuck BSY or floating bus"); return false; }

    outb(P_SECCOUNT, 0);
    outb(P_LBA0,     0);
    outb(P_LBA1,     0);
    outb(P_LBA2,     0);
    outb(P_CMD,      CMD_IDENTIFY);
    T("init: IDENTIFY issued");
    delay_400();

    st = wait_not_bsy();
    TV("init: post-IDENTIFY wait_not_bsy -> st", st);
    if(st == 0xFF){ T("init: fail: IDENTIFY stuck BSY"); return false; }

    uint8_t sig1 = inb(P_LBA1);
    uint8_t sig2 = inb(P_LBA2);
    TV("init: sig LBA1", sig1);
    TV("init: sig LBA2", sig2);
    if(sig1 || sig2){ T("init: FAIL: not ATA (ATAPI/SATA signature)"); return false; }

    st = wait_drq();
    TV("init: post-IDENIFY wait_drq -> st", st);
    if(st == 0xFF){ T("init: FAIL: no DRQ after IDENTIFY"); return false; }

    T("init: about to read 256 IDENTIFY words");
    uint16_t id[256];
    insw(P_DATA, id, 256);
    T("init: IDENTIFY data read");

    uint32_t lba28 = (uint32_t)id[60] | ((uint32_t)id[61] << 16);
    TV("init: LBA28 sector count", lba28);
    if(lba28 == 0){ T("init: FAIL: 0 sectors"); return false; }

    g_sectors = lba28;
    g_ready   = true;
    T("init: OK");
    return true;
}

uint32_t ata_sector_count(void){ return g_sectors; }

bool ata_read_sector(uint32_t lba, uint8_t *buf){
    if(!g_ready || lba >= g_sectors) return false;
    if(wait_not_bsy() == 0xFF) return false;

    outb(P_DRIVE, (uint8_t)(0xE0 | ((lba >> 24) & 0x0F)));
    delay_400();
    outb(P_SECCOUNT, 1);
    outb(P_LBA0,  (uint8_t)(lba));
    outb(P_LBA1,  (uint8_t)(lba >>  8));
    outb(P_LBA2,  (uint8_t)(lba >> 16));
    outb(P_CMD,   CMD_READ28);
    delay_400();

    if(wait_drq() == 0xFF) return false;
    insw(P_DATA, buf, 256);
    return true;
}

bool ata_write_sector(uint32_t lba, const uint8_t *buf){
    if(!g_ready || lba >= g_sectors) return false;
    if(wait_not_bsy() == 0xFF) return false;
    log_str("[ata] write begun");
    outb(P_DRIVE, (uint8_t)(0xE0 | ((lba >> 24) & 0x0F)));
    log_str("[ata] delay");
    delay_400();
    log_str("[ata] outb's");
    outb(P_SECCOUNT, 1);
    outb(P_LBA0,  (uint8_t)(lba));
    outb(P_LBA1,  (uint8_t)(lba >>  8));
    outb(P_LBA2,  (uint8_t)(lba >> 16));
    outb(P_CMD,   CMD_WRITE28);
    log_str("[ata] delay");
    delay_400();
    log_str("[ata] wait drq");
    if(wait_drq() == 0xFF) return false;
    log_str("[ata] out");
    outsw(P_DATA, buf, 256);
    log_str("[ata] wait not busy");
    if(wait_not_bsy() == 0xFF) return false;
    return true;
}

bool ata_flush(void){
    if(!g_ready) return false;
    if(wait_not_bsy() == 0xFF) return false;
    outb(P_DRIVE, 0xE0);
    delay_400();
    outb(P_CMD, CMD_FLUSH);
    if(wait_not_bsy() == 0xFF) return false;
    return true;
}