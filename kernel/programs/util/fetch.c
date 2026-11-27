#include "fetch.h"
#include "../../../drivers/display.h"
#include "../../../libs/util/info.h"
static void print_number(unsigned int number) {
    char buffer[12];
    int i = 0;

    if (number == 0) {
        print("", "0", 0x0F, 0x0F);
        return;
    }

    while (number > 0) {
        buffer[i++] = '0' + number % 10;
        number /= 10;
    }

    while (i > 0) {
        char c[2] = {buffer[--i], '\0'};
        print("", c, 0x0F, 0x0F);
    }
}
bool fetch_init(ProgramArgs args){
    print("","\n                                          @#               \n                                          -@@@             \n          @@@@@@@.    %@@@@@@.    @@@@@@@.   @@@-          \n        .@@.    @@# :@@.    @@@ :@@.    @@@    *@@@        \n        @@       @@ @@       #@+@@       *@*     .@@@.     \n    @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@.    \n        .@@.    @@# @@@     :@@ %@@     .@@     +@@@       \n          @@@@@@@.   :@@@@@@@@   .@@@@@@@@    @@@+         \n                                           .@@@.           \n                                         +@@@                                                              ",0x00,0x0A);
    print("","\n------------------------------------------------------------------------" ,0x0F,0x0F);
    char vendor[13];
    char brand[49];

    cpu_get_vendor(vendor);
    cpu_get_brand(brand);
    print("\nOS: ", "GloriousOS v0.0.1", 0x0F, 0x0F);
    print("\nCPU: ", brand, 0x0F, 0x0F);
    print("\nVendor: ", vendor, 0x0F, 0x0F);

    print("\nLogical cores: ", "", 0x0F, 0x0F);
    print_number(cpu_get_cores());

    print("\nRAM: ", "", 0x0F, 0x0F);
    print_number(ram_get_total_mb());
    print(" MB", "", 0x0F, 0x0F);

    print("\n------------------------------------------------------------------------", "", 0x0F, 0x0F);
    program_current_kill();

    return true;
}