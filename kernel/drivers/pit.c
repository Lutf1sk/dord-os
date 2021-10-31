#include <asm.h>
#include <debug_io.h>

#include <drivers/pit.h>

#include <proc.h>

volatile u32 pit_systime_msec = 0;

#include <proc.h>

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
	dbg_puts("\nSetting PIT intervals...\n");
	dbg_printf(DBG_GRY"PIT0: %udhz\n"DBG_RST, 1000);
	dbg_printf(DBG_GRY"PIT2: %udhz\n"DBG_RST, 1000);

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

