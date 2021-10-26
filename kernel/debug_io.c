#include "debug_io.h"

#include "com.h"

usz dbg_write(void* usr, void* str, usz len) {
	u8* it = str;
	for (usz i = 0; i < len; ++i)
		com_write_b(COM1_PORT, *(it++));
	return len;
}

usz dbg_puts(char* str) {
	char* it = str;
	while (*it)
		com_write_b(COM1_PORT, *(it++));
	return it - str;
}

usz dbg_printf(char* fmt, ...) {
	va_list argl;
	va_start(argl, fmt);
	usz bytes = io_vprintf(dbg_write, null, fmt, argl);
	va_end(argl);
	return bytes;
}

