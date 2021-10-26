#include "debug_io.h"

#include "com.h"

#include <stdarg.h>

void dbg_put_str(const char* str) {
	const char* it = str;
	while (*it)
		com_write_b(COM1_PORT, *(it++));
}

static char hex_char[16] = { '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'A', 'B', 'C', 'D', 'E', 'F' };

void dbg_put_hex(u32 n) {
	char buf[32], *it = buf + 31;
	*it-- = 0; // Null terminate buffer

	// Fill buffer backwards with the lowest nibble of n
	while (n > 0x0F) {
		*it-- = hex_char[n & 0x0F];
		n >>= 4;
	}
	*it = hex_char[n];

	dbg_put_str(it);
}

void dbg_put_i32(i32 n) {
	char buf[32], *it = buf + 31;
	*it-- = 0;

	// Handle signed values
	if (n < 0) {
		n = -n;
		com_write_b(COM1_PORT, '-');
	}

	// Fill buffer backwards with the remainder of n/10
	while (n >= 10) {
		*it-- = n % 10 + '0';
		n /= 10;
	}
	*it = n + '0';

	dbg_put_str(it);
}

void dbg_printf(const char* fmt, ...) {
	va_list list;
	va_start(list, fmt);

	const char* it = fmt;

	while (*it) {
		// Print current char if the character is not the start of a format
		if (*it != '%') {
			com_write_b(COM1_PORT, *(it++));
			continue;
		}

		++it; // Consume '%'

		switch (*(it++)) {
		case 'i': {
			i32 val = va_arg(list, i32);
			dbg_put_i32(val);
		}	break;

		case 's': {
			const char* val = va_arg(list, const char*);
			dbg_put_str(val);
		}	break;

		case 'c': {
			i32 val = va_arg(list, i32);
			com_write_b(COM1_PORT, val);
		}	break;


		case 'p': {
			u32 val = va_arg(list, u32);
			dbg_put_hex(val);
		}	break;

		case '%':
			com_write_b(COM1_PORT, '%');
			break;

		default:
			break;
		}

	}

	va_end(list);
}


