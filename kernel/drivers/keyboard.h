#ifndef KEYBOARD_H
#define KEYBOARD_H

#include <common.h>

void kb_handle_interrupt(void);

void kb_initialize(u8 port);

#endif
