#include "idt.h"
#include "pic.h"
#include "pit.h"

#include "../drivers/display.h"
#include "../drivers/keyboard.h"
#include "../drivers/disk/ata.h"

#include "../libs/util/io.h"
#include "../libs/util/utils.h"

#include "../libs/util/log.h"

static uint8_t write_buffer[ATA_SECTOR_SIZE];
static uint8_t read_buffer[ATA_SECTOR_SIZE];

static void disk_test(void)
{
    print("[DISK] ", "Starting ATA test...\n", 0x0B, 0x0F);
    log_str("[disk] starting ATA test\n");

    
    cli();

    if (!ata_init()) {
        print("[DISK] ", "ATA init FAILED\n", 0x0C, 0x0F);
        log_str("[disk] ATA init FAILED\n");
        sti();
        return;
    }
    print("[DISK] ", "ATA init OK\n", 0x0A, 0x0F);

    if (ata_sector_count() == 0) {
        print("[DISK] ", "0 sectors\n", 0x0C, 0x0F);
        sti();
        return;
    }
    print("[DISK] ", "Sector count nonzero\n", 0x0A, 0x0F);
    log_str("[DISC] memsets");
    memset(write_buffer, 0, sizeof(write_buffer));
    memset(read_buffer,  0, sizeof(read_buffer));
    memcpy(write_buffer, "funny file contents here", 24);
    log_str("[DISC] write");
    print("[DISK] ", "writing LBA 10...\n", 0x0B, 0x0F);
    if (!ata_write_sector(10, write_buffer)) {
        print("[DISK] ", "write FAILED\n", 0x0C, 0x0F);
        log_str("[disk] write FAILED\n");
        sti();
        return;
    }

    print("[DISK] ", "reading LBA 10...\n", 0x0B, 0x0F);
    if (!ata_read_sector(10, read_buffer)) {
        print("[DISK] ", "read FAILED\n", 0x0C, 0x0F);
        log_str("[disk] read FAILED\n");
        sti();
        return;
    }
    if (memcmp(write_buffer, read_buffer, ATA_SECTOR_SIZE) != 0) {
        print("[DISK] ", "MISMATCH\n", 0x0C, 0x0F);
        log_str("[disk] MISMATCH\n");
        sti();
        return;
    }

    print("[DISK] ", "sector R/W OK\n", 0x0A, 0x0F);
    log_str("[disk] sector R/W OK\n");

    print("[DISK] ", "flushing...\n", 0x0B, 0x0F);
    if (!ata_flush()) {
        print("[DISK] ", "flush FAILED\n", 0x0C, 0x0F);
        log_str("[disk] flush FAILED\n");
        sti();
        return;
    }
    print("[DISK] ", "all disk tests passed\n", 0x0A, 0x0F);
    log_str("[disk] all tests passed\n");

    sti();
}

void kmain(void)
{
    log_init();
    log_str("\n[boot] kmain entered\n");

    display_init();
    print("[Boot] ", "booting sequence started\n", 0x0B, 0x0F);

    log_str("[boot] idt_init\n");
    idt_init();

    log_str("[boot] pic_remap\n");
    pic_remap(0x20, 0x28);

    log_str("[boot] pit_init\n");
    pit_init(1000);

    log_str("[boot] keyboard_init\n");
    keyboard_init();

    log_str("[boot] unmask IRQ0 and IRQ1\n");
    pic_clear_mask(0);
    pic_clear_mask(1);

    log_str("[boot] sti\n");
    sti();

    log_str("[boot] entering disk_test\n");
    disk_test();
    log_str("[boot] disk_test returned\n");

    uint32_t last_hb = get_ticks();
    char ch;
    char one_char[2] = {0, 0};

    log_str("[boot] main loop\n");

    for (;;) {
        while (kbd_pop_char(&ch)) {
            one_char[0] = ch;
            print("", one_char, 0x0F, 0x0F);
        }

        uint32_t now = get_ticks();
        if ((uint32_t)(now - last_hb) >= 5000U) {
            last_hb = now;
            print("\n[hi] ", "refreshement\n", 0x0F, 0x0A);
            log_str("[hi] heartbeat\n");
        }
        hlt();
    }
}