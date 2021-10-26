#ifndef TERMINAL_H
#define TERMINAL_H

#include "common.h"

#define TERMINAL_BUF ((u16*)0xB8000)

#define TERMINAL_HEIGHT 25
#define TERMINAL_WIDTH 80
#define TERMINAL_SIZE (TERMINAL_WIDTH * TERMINAL_HEIGHT)

#define TAB_SIZE 4

extern u32 terminal_pos;
extern u16 terminal_color;

void terminal_put_char(char c);
void terminal_put_str(const char* str);
void terminal_put_hex(u32 val);
void terminal_put_i32(i32 val);

void terminal_printf(const char* fmt, ...);

void terminal_clear(void);

#endif
