
#include "pit.h"

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

#include "asm.h"

static volatile u32 pit_systime_msec = 0;

#include "debug_io.h"

void pit_handle_interrupt(void) {
	++pit_systime_msec;
}

static
void pit_set_frequency(u8 channel, u32 hz) {
	if (!hz)
		hz = 1;

	u32 divisor = (7159090 + 6/2) / (6 * hz);

	u16 data_port = PIT_DATA0 + channel;
	u8 command = (channel << 6) | PIT_MODE3 | PIT_LOHIBYTE;

	outb(PIT_CMD, command);
	outb(data_port, divisor);
	outb(data_port, divisor >> 8);
}

void pit_initialize(void) {
	dbg_put_str("\nSetting PIT intervals...\n");
	dbg_printf(DBG_GRY"PIT0: %ihz\n"DBG_RST, 1000);
	dbg_printf(DBG_GRY"PIT2: %ihz\n"DBG_RST, 1000);

	pit_set_frequency(0, 1000);
	//pit_set_frequency(1, 1000); // This channel is unnecessary on modern PCs
	pit_set_frequency(2, 1000);
}

void pit_sleep_msec(u32 msec) {
	volatile u32 wait_until = pit_systime_msec + msec;
	while (pit_systime_msec <= wait_until)
		;
}

u32 pit_time_msec(void) {
	return pit_systime_msec;
}

