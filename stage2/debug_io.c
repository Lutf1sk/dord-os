#include "debug_io.h"
#include "asm.h"
#include "io.h"

#define COM1_PORT 0x3F8

static
void com_write_b(u8 data) {
	while (!(inb(COM1_PORT + 5) & 0x20))
		;
	outb(COM1_PORT, data);
}

usz dbg_write(void* usr, void* data, usz len) {
	u8* it = data;
	for (usz i = 0; i < len; ++i)
		com_write_b(*it++);
	return len;
}

usz dbg_puts(char* str) {
	char* it = str;
	while (*it)
		com_write_b(*(it++));
	return it - str;
}

usz dbg_printf(char* fmt, ...) {
	va_list argl;
	va_start(argl, fmt);
	usz bytes = io_vprintf(dbg_write, null, fmt, argl);
	va_end(argl);
	return bytes;
}


