
#ifndef GLORIOUS_SHELL_H
#define GLORIOUS_SHELL_H
#include "../../drivers//keyboard.h"
bool shell_init(void);
void shell_input(KeyEvent);
void shell_run();

#endif
