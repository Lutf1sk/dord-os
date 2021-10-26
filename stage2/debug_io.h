#ifndef DEBUG_IO_H
#define DEBUG_IO_H

#include "common.h"

#define DBG_RED "\x1b[31m"
#define DBG_GRN "\x1b[32m"
#define DBG_YLW "\x1b[33m"
#define DBG_BLU "\x1b[34m"
#define DBG_MGN "\x1b[35m"
#define DBG_CYN "\x1b[36m"
#define DBG_GRY "\x1b[37m"
#define DBG_RST "\x1b[0m"

void dbg_put_str(const char* str);
void dbg_put_hex(u32 val);
void dbg_put_i32(i32 val);
void dbg_printf(const char* fmt, ...);

#endif
