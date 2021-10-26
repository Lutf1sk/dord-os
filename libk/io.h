#ifndef IO_H
#define IO_H

#include "common.h"

#include <stdarg.h>

typedef usz (*io_callback_t)(void* usr, void* data, usz len);

usz io_vprintf(io_callback_t callb, void* usr, char* fmt, va_list argl);
usz io_printf(io_callback_t callb, void* usr, char* fmt, ...);

#endif
