#ifndef GLORIOUS_PROGRAM_H
#define GLORIOUS_PROGRAM_H

#include <stdbool.h>
#include "../drivers//keyboard.h"

#define MAX_PROGRAMS 256

typedef struct Program {
    void (*input)(KeyEvent);
    void (*update)(void);
    bool (*init)(void);
} Program;
typedef struct {
    Program programs[MAX_PROGRAMS];
    unsigned int count;
} ProgramBuffer;
void program_current_kill();
void program_start(Program pr);
void programs_update();
void programs_input(KeyEvent k);
void programs_init();

// util functions if a program doesn't need that branch
void pr_empty_init();
void pr_empty_input(KeyEvent k);
void pr_empty_update();
#endif