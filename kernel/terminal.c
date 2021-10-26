#include "terminal.h"

#include "memory.h"
#include "asm.h"
#include "io.h"

#define TERMINAL_BUF ((u16*)0xB8000)

#define TERMINAL_HEIGHT 25
#define TERMINAL_WIDTH 80
#define TERMINAL_SIZE (TERMINAL_WIDTH * TERMINAL_HEIGHT)

#define TAB_SIZE 4

u32 terminal_pos = 0;
u16 terminal_color = 0x0F;

static
void terminal_update_cursor_pos() {
	outb(0x3D4, 0x0F);
	outb(0x3D5, (u8) (terminal_pos & 0xFF));
	outb(0x3D4, 0x0E);
	outb(0x3D5, (u8) ((terminal_pos >> 8) & 0xFF));
}

void terminal_putc(char c) {
	switch (c) {
	case '\n': {
		u32 line_begin_offs = terminal_pos % TERMINAL_WIDTH;
		terminal_pos += TERMINAL_WIDTH - line_begin_offs;
	}	break;

	case '\t': {
		u32 line_begin_offs = terminal_pos % TERMINAL_WIDTH;
		u32 tab_offs = line_begin_offs % TAB_SIZE;
		terminal_pos += TAB_SIZE - tab_offs;
	}	break;

	case '\b': {
		if (terminal_pos)
			--terminal_pos;
		TERMINAL_BUF[terminal_pos] = terminal_color << 8;
	} break;

	default:
		TERMINAL_BUF[terminal_pos++] = c | terminal_color << 8;
		break;
	}

	if (terminal_pos >= TERMINAL_SIZE) {
		mmove16(TERMINAL_BUF, &TERMINAL_BUF[TERMINAL_WIDTH], TERMINAL_SIZE - TERMINAL_WIDTH);
		mset16(&TERMINAL_BUF[TERMINAL_WIDTH * (TERMINAL_HEIGHT - 1)], 0, TERMINAL_WIDTH);
		terminal_pos -= TERMINAL_WIDTH;
	}

	terminal_update_cursor_pos();
}

usz terminal_puts(char* str) {
	char* it = str;
	while (*it)
		terminal_putc(*it++);
}

usz terminal_write(void* usr, void* data, usz len) {
	char* it = data;
	for (usz i = 0; i < len; ++i)
		terminal_putc(*it++);
	return len;
}

usz terminal_printf(char* fmt, ...) {
	va_list argl;
	va_start(argl, fmt);
	usz bytes = io_vprintf(terminal_write, null, fmt, argl);
	va_end(argl);
	return bytes;
}

void terminal_clear(void) {
	mset16(TERMINAL_BUF, 0, TERMINAL_SIZE);
	terminal_pos = 0;
}
