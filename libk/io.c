#include "io.h"
#include "memory.h"

static
usz io_printuq(io_callback_t callb, void* usr, u64 n) {
	char buf[32], *end = buf + sizeof(buf) - 1, *it = end;

	// Fill buffer backwards with the remainder of n/10
	while (n >= 10) {
		*it-- = n % 10 + '0';
		n /= 10;
	}
	*it-- = n + '0';

	usz len = end - it;
	return callb(usr, it, len + 1);
}

static
usz io_printiq(io_callback_t callb, void* usr, i64 n) {
	static char minus = '-';
	usz sign = 0;

	// Handle signed values
	if (n < 0) {
		n = -n;
		callb(usr, &minus, 1);
		sign = 1;
	}

	return io_printuq(callb, usr, n) + sign;
}

static char io_hex_char[16] = { '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'A', 'B', 'C', 'D', 'E', 'F' };

static
usz io_printhq(io_callback_t callb, void* usr, u64 n) {
	char buf[32], *end = buf + sizeof(buf) - 1, *it = end;

	// Fill buffer backwards with the lowest nibble of n
	while (n > 0x0F) {
		*it-- = io_hex_char[n & 0x0F];
		n >>= 4;
	}
	*it-- = io_hex_char[n];

	usz len = end - it;
	return callb(usr, it, len + 1);
}

usz io_vprintf(io_callback_t callb, void* usr, char* fmt, va_list argl) {
	char* it = fmt;

	while (*it) {
		char* start = it;

		if (*it != '%') {
			while (*it && *it != '%')
				++it;
			callb(usr, start, it - start);
			continue;
		}

		++it; // Consume '%'

		switch (*(it++)) {
		case 'i': {
			i32 val = va_arg(argl, i32);
			io_printiq(callb, usr, val);
		}	break;

		case 's': {
			char* val = va_arg(argl, char*);
			callb(usr, val, strlen(val));
		}	break;

		case 'c': {
			i32 val = va_arg(argl, i32);
			callb(usr, &val, 1);
		}	break;


		case 'p': {
			u32 val = va_arg(argl, u32);
			io_printhq(callb, usr, val);
		}	break;

		case '%':
			callb(usr, it - 1, 1);
			break;

		default:
			break;
		}

	}
}

FLATTEN
usz io_printf(io_callback_t callb, void* usr, char* fmt, ...) {
	va_list argl;
	va_start(argl, fmt);
	usz bytes = io_vprintf(callb, usr, fmt, argl);
	va_end(argl);
	return bytes;
}

