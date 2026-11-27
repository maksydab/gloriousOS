#include "../../drivers/display.h"
#include <stdbool.h>
#include "../../libs/util/stdtypes.h"
#include "../../libs/util/utils.h"
#include "../../libs/util/log.h"
#include "shell.h"

bool vis = true;
String buf;
char one_char[2] = {0, 0};
int cursorpo=0;
int promptposx=0;
int promptposy=0;
int longest=0;
bool shell_init(void) {
    log_str("[shell] starting shell\n");
    vis = true;

    if (!string_init(&buf, 256))
        return false;

    if (vis) {
        log_str("[shell] starting visual systems\n");
        clear(0x07);
        print("\n>>> ", "", 0x0E, 0x0F);

        promptposx = get_cursor_x();
        promptposy = get_cursor_y();
    }

    return true;
}
void shell_execute(){
    if (string_compare_cstr(&buf, "echo")==0){
        print("\n","hi",0x0F,0x0F);
        log_str("[ECHO] echoing\n");
    }else{
        print("\n","error: command not found",0x0F,0x04);
    }
    print("\n>>> ", "", 0x0E, 0x0F);

    promptposx = get_cursor_x();
    promptposy = get_cursor_y();
    string_init(&buf, 256);
}
void shell_input(KeyEvent event) {
    switch (event.code) {
        case KEY_CHAR:
            string_append_char(&buf, event.ch);
            one_char[0] = event.ch;
            
            if (vis)
                print("", one_char, 0x0C, 0x0F);
            cursorpo++;
            if (string_length(&buf)>longest){
                longest=string_length(&buf);
            }
            break;

        case KEY_BACKSPACE:
            if (cursorpo > 0) {
                cursorpo--;
                string_rem(&buf, cursorpo, cursorpo);
            }
            break;
        case KEY_ENTER:
            shell_execute();

        default:
            break;
    }
    if (vis){
        set_cursor(promptposx, promptposy);
        // truly programmer way to do it
        for (int i = 0; i < longest; i++)
            print("", " ", 0x0F, 0x0F);
        set_cursor(promptposx, promptposy);
        print_string(&buf, 0x0F);
    }
}
void shell_run(){

}