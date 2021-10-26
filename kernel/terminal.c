#include "terminal.h"

#include "memory.h"
#include "asm.h"

#include <stdarg.h>

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

void terminal_put_char(char c) {
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

void terminal_put_str(const char* str) {
	const char* it = str;
	while (*it)
		terminal_put_char(*it++);
}

void terminal_put_hex(u32 val) {
	char buf[32];
	buf[31] = 0;

	char* it = buf + 31;

	u32 n = val;
	while (n >= 16) {
		--it;
		u32 rem = n % 16;
		n /= 16;
		*it = rem > 9 ? 'A' + rem - 10 : '0' + rem;
	}
	--it;
	u32 rem = n % 16;
	*it = rem > 9 ? 'A' + rem - 10 : '0' + rem;

	terminal_put_str(it);
}

void terminal_put_i32(i32 val) {
	char buf[32];
	buf[31] = 0;

	char* it = buf + 31;

	i32 n = val;
	while (n >= 10) {
		--it;
		i32 rem = n % 10;
		n /= 10;
		*it = '0' + rem;
	}
	--it;
	i32 rem = n % 10;
	*it = '0' + rem;

	terminal_put_str(it);
}

void terminal_printf(const char* fmt, ...) {
	va_list list;
	va_start(list, fmt);

	const char* it = fmt;

	while (*it) {
		if (*it != '%') {
			terminal_put_char(*(it++));
			continue;
		}

		++it;

		switch (*(it++)) {
		case 'i': {
			i32 val = va_arg(list, i32);
			terminal_put_i32(val);
		}	break;

		case 's': {
			const char* val = va_arg(list, const char*);
			terminal_put_str(val);
		}	break;

		case 'c': {
			i32 val = va_arg(list, i32);
			terminal_put_char(val);
		}	break;

		case 'v': {
			u8 clr = va_arg(list, u32);
			terminal_color = clr;
		}	break;

		case 'p': {
			u32 val = va_arg(list, u32);
			terminal_put_hex(val);
		}	break;

		case '%':
			terminal_put_char('%');
			break;

		default:
		break;

		}

	}

	va_end(list);
}

void terminal_clear(void) {
	mset16(TERMINAL_BUF, 0, TERMINAL_SIZE);
	terminal_pos = 0;
}
