#include "programs.h"
#include "../drivers/keyboard.h"
#include "../libs/util/log.h"
ProgramBuffer pro_buffer = {0};
ProgramArgs empty_args;
// program handling helpers

void  _add_program(ProgramBuffer *buffer, Program program) {
    if (buffer->count >= MAX_PROGRAMS)
        return;

    buffer->programs[buffer->count++] = program;
}
bool _remove_program(ProgramBuffer *buffer, unsigned int index) {
    if (index >= buffer->count)
        return false;

    for (unsigned int i = index; i < buffer->count - 1; i++) {
        buffer->programs[i] = buffer->programs[i + 1];
    }

    buffer->count--;

    return true;
}
void program_current_kill(){
    _remove_program(&pro_buffer, pro_buffer.count-1);
}
void program_start(Program pr, ProgramArgs args) {
    log_str("[programs] started a program\n");

    _add_program(&pro_buffer, pr);

    if (pro_buffer.count > 0) {
        pr.init(args);
    }
}
void programs_update(){
    if (pro_buffer.count>0){
        pro_buffer.programs[pro_buffer.count-1].update();
    }
}
void programs_input(KeyEvent k){
    if (pro_buffer.count>0){
        pro_buffer.programs[pro_buffer.count-1].input(k);
    }
}
void programs_init(){
    
    if (pro_buffer.count>0){
        pro_buffer.programs[pro_buffer.count-1].init(empty_args);
    }
}
void pr_empty_update(){
    return;
}
void pr_empty_input(KeyEvent k){
    return;
}
void pr_empty_init(ProgramArgs args) {
    return;
}
ProgramArgs pr_get_empty_args(){
    return empty_args;
}