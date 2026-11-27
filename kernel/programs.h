#ifndef GLORIOUS_PROGRAM_H
#define GLORIOUS_PROGRAM_H

#include <stdbool.h>
#include "../drivers/keyboard.h"

#define MAX_PROGRAMS 256
#define MAX_ARGS 16

typedef struct {
    int argc;
    char *argv[MAX_ARGS];
} ProgramArgs;

typedef struct Program {
    void (*input)(KeyEvent);
    void (*update)(void);
    bool (*init)(ProgramArgs args);
} Program;

typedef struct {
    Program programs[MAX_PROGRAMS];
    unsigned int count;
} ProgramBuffer;

void program_current_kill();
void program_start(Program pr, ProgramArgs args);

void programs_update();
void programs_input(KeyEvent k);
void programs_init();
// utils for empty functions if a function is unneeded in the program
void pr_empty_init(ProgramArgs args);
void pr_empty_input(KeyEvent k);
void pr_empty_update();
ProgramArgs pr_get_empty_args();

#endif