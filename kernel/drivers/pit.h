#ifndef PIT_H
#define PIT_H

#include <common.h>

#define PIT_DATA0 0x40
#define PIT_DATA1 0x41
#define PIT_DATA2 0x42

#define PIT_CMD 0x43

#define PIT_SEL0 (0b00 << 6)
#define PIT_SEL1 (0b01 << 6)
#define PIT_SEL2 (0b10 << 6)
#define PIT_READBACK (0b11 << 6)

#define PIT_LATCH_COUNT (0b00 << 4)
#define PIT_LOBYTE (0b01 << 4)
#define PIT_HIBYTE (0b10 << 4)
#define PIT_LOHIBYTE (0b11 << 4)

#define PIT_MODE0 (0b000 << 1)
#define PIT_MODE1 (0b001 << 1)
#define PIT_MODE2 (0b010 << 1)
#define PIT_MODE3 (0b011 << 1)
#define PIT_MODE4 (0b100 << 1)
#define PIT_MODE5 (0b101 << 1)

void pit_handle_interrupt(void);

void pit_initialize(void);

void pit_sleep_msec(u32 msec);
u32 pit_time_msec(void);

#endif
