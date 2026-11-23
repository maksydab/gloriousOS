
#include "idt.h"
#include "pic.h"
#include "pit.h"

#include "../drivers/display.h"
#include "../drivers/keyboard.h"

#include "../libs/util/io.h"

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
