#include "../../drivers/display.h"
#include <stdbool.h>
#include "../../libs/util/stdtypes.h"
#include "../../libs/util/utils.h"
#include "../../libs/util/log.h"
#include "shell.h"
#include "../programs.h"

// declare programs that are able to be invoked using shell
#include "util/fetch.h"
static Program fetch = {
    .init = fetch_init,
    .input = pr_empty_input,
    .update = pr_empty_update,
};

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

void shell_execute() {
    char *argv[MAX_ARGS];
    int argc = 0;
    char *input = buf.data;
    char *token = strtok(input, " ");
    while (token != NULL && argc < MAX_ARGS) {
        argv[argc++] = token;
        token = strtok(NULL, " ");
    }
    if (argc == 0)
        return;
    ProgramArgs args = {
        .argc = argc
    };
    for (int i = 0; i < argc; i++)
        args.argv[i] = argv[i];
    if (strcmp(argv[0], "echo") == 0) {
        for (int i = 1; i < argc; i++) {
            print("", argv[i], 0x0F, 0x0F);
            print("", " ", 0x0F, 0x0F);
        }
    } else if (strcmp(argv[0], "fetch") == 0) {
        program_start(fetch, args);

    } else {
        print("\n", "error: command not found", 0x0F, 0x04);
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