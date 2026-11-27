
#ifndef GLORIOUS_SHELL_H
#define GLORIOUS_SHELL_H
#include "../../drivers/keyboard.h"
#include "../../kernel/programs.h"
bool shell_init(ProgramArgs);
void shell_input(KeyEvent);
void shell_run();

#endif
