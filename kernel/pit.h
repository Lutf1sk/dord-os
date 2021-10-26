#ifndef PIT_H
#define PIT_H

#include "common.h"

void pit_handle_interrupt(void);

void pit_initialize(void);

void pit_sleep_msec(u32 msec);
u32 pit_time_msec(void);

#endif
