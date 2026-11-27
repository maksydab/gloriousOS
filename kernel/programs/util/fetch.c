#include "fetch.h"
#include "../../../drivers/display.h"

bool fetch_init(){
    print("","\nHi from fetch",0x00,0x0A);
    program_current_kill();
}