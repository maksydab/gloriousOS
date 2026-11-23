
#include "idt.h"
#include "pic.h"
#include "pit.h"

#include "../drivers/display.h"
#include "../drivers/keyboard.h"
#include "../drivers/disk/ata.h"


#include "../libs/util/io.h"

#include "../libs/util/utils.h"

static uint8_t write_buffer[512];
static uint8_t read_buffer[512];

static void disk_test(void) {
    print("[DISK] ", "entered disk_test\n", 0x0B, 0x0F);

    memset(write_buffer, 0, sizeof(write_buffer));
    memset(read_buffer, 0, sizeof(read_buffer));

    print("[DISK] ", "buffers initialized\n", 0x0B, 0x0F);

    const char message[] = "funny file contents here";

    for (uint32_t i = 0; i < sizeof(message); i++) {
        write_buffer[i] = (uint8_t)message[i];
    }

    print("[DISK] ", "writing sector...\n", 0x0B, 0x0F);

    if (!ata_write_sector(10, write_buffer)) {
        print("[DISK] ", "write failed!\n", 0x0C, 0x0F);
        return;
    }

    print("[DISK] ", "write completed\n", 0x0A, 0x0F);

    if (!ata_read_sector(10, read_buffer)) {
        print("[DISK] ", "read failed!\n", 0x0C, 0x0F);
        return;
    }

    print("[DISK] ", "read completed\n", 0x0A, 0x0F);

    if (memcmp(write_buffer, read_buffer, 512) == 0) {
        print("[DISK] ", "read/write successful!\n", 0x0A, 0x0F);
    } else {
        print("[DISK] ", "data mismatch!\n", 0x0C, 0x0F);
    }
}
void kmain(void) {
    display_init();

    print("[Boot] ", "Booting sequence started\n", 0x0B, 0x0F);

    print(
        "[Boot] ",
        "initializing IDT, PIC, PIT und keyboard\n",
        0x0B,
        0x0F
    );

    // interrupt descriptor table
    idt_init();

    // remap the PIC
    pic_remap(0x20, 0x28);

    // initialize timer at 1000 Hz
    pit_init(1000);

    // initialize keyboard
    keyboard_init();

    // unmask the timer IRQ
    pic_clear_mask(0);

    // enables interrupts
    sti();
    // disk_test(); not ready
    uint32_t last_hb = get_ticks();

    char ch;
    char one_char[2] = {0, 0};

    for (;;) {

        // keyboard input
        while (kbd_pop_char(&ch)) {
            one_char[0] = ch;
            print("", one_char, 0x0F, 0x0F);
        }

        // heartbeat every 5 seconds
        uint32_t current_ticks = get_ticks();

        if ((uint32_t)(current_ticks - last_hb) >= 5000U) {
            last_hb = current_ticks;

            print(
                "\n[hi] ",
                "refreshement\n",
                0x0F,
                0x0A
            );
        }

        // wait for next upon interrupt
        hlt();
    }
}
