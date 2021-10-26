#ifndef COM_H
#define COM_H

#include "common.h"

#define COM1_PORT 0x3F8
#define COM2_PORT 0x2F8
#define COM3_PORT 0x3E8
#define COM4_PORT 0x2E8

void com_write_b(u16 port, u8 data);

#endif
