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

#endif