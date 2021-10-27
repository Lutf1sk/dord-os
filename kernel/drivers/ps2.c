#include <asm.h>
#include <debug_io.h>

#include <drivers/ps2.h>
#include <drivers/pit.h>

u16 ps2_p1 = 1, ps2_p2 = 1;

#define PS2_TIMEOUT_MSEC 10

i16 ps2_send_cmd(u8 cmd) {
	u32 timeout_at = pit_time_msec() + PS2_TIMEOUT_MSEC;
	while (inb(PS2_STATUS) & PS2_SR_INP) {
		if (pit_time_msec() >= timeout_at) {
			dbg_puts(DBG_YLW"ps2_send_cmd: timed out\n"DBG_RST);
			return -1;
		}
	}
	outb(PS2_CMD, cmd);
	return 0;
}

i16 ps2_send_data(u8 data) {
	u32 timeout_at = pit_time_msec() + PS2_TIMEOUT_MSEC;
	while (inb(PS2_STATUS) & PS2_SR_INP) {
		if (pit_time_msec() >= timeout_at) {
			dbg_puts(DBG_YLW"ps2_send_data: timed out\n"DBG_RST);
			return -1;
		}
	}
	outb(PS2_DATA, data);
	return 0;
}

i16 ps2_recv(void) {
	u32 timeout_at = pit_time_msec() + PS2_TIMEOUT_MSEC;
	while (!(inb(PS2_STATUS) & PS2_SR_OUT)) {
		if (pit_time_msec() >= timeout_at) {
			dbg_puts(DBG_YLW"ps2_recv: timed out\n"DBG_RST);
			return -1;
		}
	}
	return inb(PS2_DATA);
}

i16 ps2_send_port(u8 port, u8 cmd) {
	if (port) {
		if (ps2_send_cmd(PS2_CMD_WRITE_P2_INP) < 0)
			return -1;
		inb(PS2_DATA);
	}
	return ps2_send_data(cmd);
}

void ps2_disable_port(u8 port) {
	if (port && ps2_send_cmd(PS2_CMD_DISABLE_P2) < 0)
		dbg_puts(DBG_RED"Failed to disable PS/2 port 2\n"DBG_RST);
	else if (ps2_send_cmd(PS2_CMD_DISABLE_P1) < 0)
		dbg_puts(DBG_RED"Failed to disable PS/2 port 1\n"DBG_RST);
}

void ps2_enable_port(u8 port) {
	if (port && ps2_send_cmd(PS2_CMD_ENABLE_P2) < 0)
		dbg_puts(DBG_RED"Failed to enable PS/2 port 2\n"DBG_RST);
	else if (ps2_send_cmd(PS2_CMD_ENABLE_P1) < 0)
		dbg_puts(DBG_RED"Failed to enable PS/2 port 1\n"DBG_RST);
}

char* ps2_dev_str(u16 type) {
	switch (type) {
	case 0: return "Generic PS/2 Mouse";
	case 3: return "Z-Axis Mouse";
	case 4: return "5 Button Mouse";
	case 0xAB83: return "MF2 Keyboard";
	case 0xAB41: case 0xABC1:
		return "Translated MF2 Keyboard";
	default:
		return "Unknown PS/2 device";
	}
}

void ps2_enable_irqs(void) {
	ps2_send_cmd(PS2_CMD_READ);
	i16 cfg = ps2_recv();
	cfg |= PS2_CFG_P1_IRQ | PS2_CFG_P2_IRQ;
	ps2_send_cmd(PS2_CMD_WRITE);
	ps2_send_data(cfg);
}

static
u16 ps2_identify(u8 dev) {
	u16 type = 0;
	ps2_send_port(dev, PS2_DCMD_IDENTIFY);
	ps2_recv(); // ACK
	type = ps2_recv();
	if (type == 0xAB)
		type = (type << 8) | ps2_recv();
	return type;
}

void ps2_initialize(void) {
	dbg_puts("\nInitializing PS/2 controller...\n");

	u8 dual_channel = 1;
	ps2_p1 = PS2_DEV_NONE;
	ps2_p2 = PS2_DEV_NONE;

	// Disable devices
	ps2_disable_port(PS2_PORT1);
	ps2_disable_port(PS2_PORT2);

	// Flush output buffer
	inb(PS2_DATA);

	// Disable IRQs and translation
	ps2_send_cmd(PS2_CMD_READ);
	i16 cfg = ps2_recv();
	cfg &= ~(PS2_CFG_P1_IRQ | PS2_CFG_P2_IRQ | PS2_CFG_P1_TRANSLATION);
	if (!(cfg & PS2_CFG_DISABLE_P2_CLOCK))
		dual_channel = 0;
	ps2_send_cmd(PS2_CMD_WRITE);
	ps2_send_data(cfg);

	// Perform self test
	ps2_send_cmd(PS2_CMD_TEST_CTRL);
	if (ps2_recv() != 0x55) {
		dbg_puts(DBG_RED"Self test of PS/2 controller failed\n"DBG_RST);
		return;
	}

	// Perform interface tests
	ps2_send_cmd(PS2_CMD_TEST_P1);
	if (ps2_recv() != 0x00)
		dbg_puts(DBG_RED"Interface test of PS/2 port 1 failed\n"DBG_RST);
	if (dual_channel) {
		ps2_send_cmd(PS2_CMD_TEST_P2);
		if (ps2_recv() != 0x00) {
			dbg_puts(DBG_RED"Interface test of PS/2 port 2 failed\n"DBG_RST);
			dual_channel = 0;
		}
	}

	// Enable, reset and identify devices
	ps2_enable_port(PS2_PORT1);
	ps2_send_port(PS2_PORT1, PS2_DCMD_RESET);
	if (ps2_recv() == PS2_ACK && ps2_recv() == 0xAA) {
		ps2_p1 = ps2_identify(PS2_PORT1);
		dbg_printf(DBG_GRY"Port 1: Found '%s' (0x%hd)\n"DBG_RST, ps2_dev_str(ps2_p1), ps2_p1);
	}

	if (dual_channel) {
		ps2_enable_port(PS2_PORT2);
		ps2_send_port(PS2_PORT2, PS2_DCMD_RESET);
		if (ps2_recv() == PS2_ACK && ps2_recv() == 0xAA) {
			ps2_p2 = ps2_identify(PS2_PORT2);
			dbg_printf(DBG_GRY"Port 2: Found '%s' (0x%hd)\n"DBG_RST, ps2_dev_str(ps2_p2), ps2_p2);
		}
	}
}

