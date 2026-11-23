
#ifndef GLORIOUS_DISPLAY_H
#define GLORIOUS_DISPLAY_H

void display_init(void);

void print(
    const char *prefix,
    const char *msg,
    unsigned char prefix_color,
    unsigned char msg_color
);

#endif
