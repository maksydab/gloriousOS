#include "idt.h"
#include "pic.h"
#include "pit.h"

#include "../drivers/display.h"
#include "../drivers/keyboard.h"
#include "../drivers/disk/ata.h"

#include "../libs/util/io.h"
#include "../libs/util/utils.h"

#include "../libs/util/log.h"
#include "programs/shell.h"
#include "programs.h"

static uint8_t write_buffer[ATA_SECTOR_SIZE];
static uint8_t read_buffer[ATA_SECTOR_SIZE];
static Program shell = {
    .input = shell_input,
    .update = shell_run,
    .init = shell_init,
};

// for testing if the disc works
static void disk_test(void)
{
    clear(0x07);
    print("[Disk] ", "Starting ATA test...\n", 0x0B, 0x0F);
    log_str("[disk] starting ATA test\n");

    
    cli();

    if (!ata_init()) {
        print("[Disk] ", "ATA init FAILED\n", 0x0C, 0x0F);
        log_str("[disk] ATA init FAILED\n");
        sti();
        return;
    }
    print("[Disk] ", "ATA init OK\n", 0x0A, 0x0F);

    if (ata_sector_count() == 0) {
        print("[Disk] ", "0 sectors\n", 0x0C, 0x0F);
        sti();
        return;
    }
    print("[Disk] ", "Sector count nonzero\n", 0x0A, 0x0F);
    log_str("[Disk] memsets");
    memset(write_buffer, 0, sizeof(write_buffer));
    memset(read_buffer,  0, sizeof(read_buffer));
    memcpy(write_buffer, "funny file contents here", 24);
    log_str("[Disk] write");
    print("[Disk] ", "writing LBA 10...\n", 0x0B, 0x0F);
    if (!ata_write_sector(10, write_buffer)) {
        print("[Disk] ", "write FAILED\n", 0x0C, 0x0F);
        log_str("[disk] write FAILED\n");
        sti();
        return;
    }

    print("[Disk] ", "reading LBA 10...\n", 0x0B, 0x0F);
    if (!ata_read_sector(10, read_buffer)) {
        print("[Disk] ", "read FAILED\n", 0x0C, 0x0F);
        log_str("[disk] read FAILED\n");
        sti();
        return;
    }
    if (memcmp(write_buffer, read_buffer, ATA_SECTOR_SIZE) != 0) {
        print("[Disk] ", "MISMATCH\n", 0x0C, 0x0F);
        log_str("[disk] MISMATCH\n");
        sti();
        return;
    }

    print("[Disk] ", "sector R/W OK\n", 0x0A, 0x0F);
    log_str("[disk] sector R/W OK\n");

    print("[Disk] ", "flushing...\n", 0x0B, 0x0F);
    if (!ata_flush()) {
        print("[Disk] ", "flush FAILED\n", 0x0C, 0x0F);
        log_str("[disk] flush FAILED\n");
        sti();
        return;
    }
    print("[Disk] ", "all disk tests passed\n", 0x0A, 0x0F);
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
    KeyEvent event;


    log_str("[boot] launching shell\n");

    //shell_init(true);
    log_str("[boot] main loop\n");
    programs_init();
    program_start(shell, pr_get_empty_args());
    for (;;) {
        while (kbd_pop_event(&event)) {
            //print("", one_char, 0x0F, 0x0F);
            programs_input(event);
        }
        programs_update();
        uint32_t now = get_ticks();
        if ((uint32_t)(now - last_hb) >= 5000U) {
            last_hb = now;
            //print("\n[kernel] ", "heartbeat\n", 0x0A, 0x0F);
            log_str("[kernel] heartbeat\n");
        }
        hlt();
    }
}