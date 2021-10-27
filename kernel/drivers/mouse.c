#include <asm.h>
#include <debug_io.h>

#include <drivers/mouse.h>
#include <drivers/ps2.h>
#include <drivers/pit.h>

u32 mouse_type = PS2_DEV_GENERIC_MOUSE;
u32 mouse_packet_size = 3;
i32 mouse_x = 0, mouse_y = 0, mouse_z = 0;

u8 button_states[5] = {0};

void mouse_handle_interrupt(void) {
	static u32 packet_byte = 0;
	static u8 packet[4];

	u8 status = 0;
	while (((status = inb(PS2_STATUS)) & PS2_SR_OUT) && (status & PS2_SR_MOUSE)) {

		u8 byte = inb(PS2_DATA);
		packet[packet_byte] = byte;

		// Await next byte if the entire packet has not been collected
		if (++packet_byte < mouse_packet_size)
			continue;

		packet_byte = 0;

		u8 p0 = packet[0];
		// Store available button states
		button_states[MS_BTN_1] = p0 & MS_CPKT_LMB;
		button_states[MS_BTN_2] = p0 & MS_CPKT_RMB;
		button_states[MS_BTN_3] = p0 & MS_CPKT_MMB;

		// Bit shifting magic to sign extend the 9-bit x/y offsets from the packet
		mouse_x += (i16)((((p0 & MS_CPKT_XSIGN) << 8) | packet[1]) << 8) >> 8;
		mouse_y -= (i16)((((p0 & MS_CPKT_YSIGN) << 8) | packet[2]) << 8) >> 8;

		if (mouse_type == PS2_DEV_ZAXIS_MOUSE)
			mouse_z += (i8)packet[3];
		else if (mouse_type == PS2_DEV_5BTN_MOUSE) {
			// More bit shifting magic to sign extend the 4-bit scroll value
			mouse_z += (i8)((packet[3] & MS_5PKT_ZM) << 4) >> 4;

			u8 p3 = packet[3];
			// Store available button states
			button_states[MS_BTN_4] = p3 & MS_5PKT_4MB;
			button_states[MS_BTN_5] = p3 & MS_5PKT_5MB;
		}
	}
}

static
void mouse_set_rate(u8 port, u8 rate) {
	ps2_send_port(port, MS_CMD_SET_SAMPLE_RATE);
	if (ps2_recv() != PS2_ACK)
		dbg_puts(DBG_RED"Failed to set mouse sample rate\n"DBG_RST);
	ps2_send_port(port, rate);
}

void mouse_initialize(u8 port) {
	dbg_puts("\nInitializing mouse...\n");

	ps2_send_port(port, MS_CMD_SET_DEFAULTS);
	if (ps2_recv() != PS2_ACK)
		dbg_puts(DBG_RED"Failed to set mouse defaults\n"DBG_RST);

	ps2_send_port(port, MS_CMD_ENABLE_DREP);
	if (ps2_recv() != PS2_ACK)
		dbg_puts(DBG_RED"Could not enable data reporting\n"DBG_RST);

	// Z-Axis magic
	mouse_set_rate(port, 200);
	mouse_set_rate(port, 100);
	mouse_set_rate(port, 80);
	ps2_send_port(port, MS_CMD_GET_DEVICE_ID);
	if (ps2_recv() != PS2_ACK)
		dbg_puts(DBG_RED"Failed to get mouse device id\n"DBG_RST);

	if ((mouse_type = ps2_recv()) == 3) {
		mouse_packet_size = 4;

		// 5 Button magic
		mouse_set_rate(port, 200);
		mouse_set_rate(port, 200);
		mouse_set_rate(port, 80);
		ps2_send_port(port, MS_CMD_GET_DEVICE_ID);
		if (ps2_recv() != PS2_ACK)
			dbg_puts(DBG_RED"Failed to get mouse device id\n"DBG_RST);

		mouse_type = ps2_recv();
	}

	dbg_printf(DBG_GRY"Mouse type: '%s'\n"DBG_RST, ps2_dev_str(mouse_type));
}

