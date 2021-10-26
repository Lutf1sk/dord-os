#ifndef DEBUG_IO_H
#define DEBUG_IO_H

#include "io.h"

#define DBG_RED "\x1b[31m"
#define DBG_GRN "\x1b[32m"
#define DBG_YLW "\x1b[33m"
#define DBG_BLU "\x1b[34m"
#define DBG_MGN "\x1b[35m"
#define DBG_CYN "\x1b[36m"
#define DBG_GRY "\x1b[37m"
#define DBG_RST "\x1b[0m"

usz dbg_write(void* usr, void* str, usz len);
usz dbg_puts(char* str);

usz dbg_printf(char* fmt, ...);

#endif
