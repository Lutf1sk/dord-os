#include <asm.h>
#include <debug_io.h>

#include <drivers/keyboard.h>
#include <drivers/ps2.h>

void kb_handle_interrupt(void) {
	u8 scan_code = inb(PS2_DATA);

	static u8 lshift_pressed = 0;
	static u8 rshift_pressed = 0;

	switch (scan_code) {
	case 0x2A: lshift_pressed = 1; break;
	case 0xAA: lshift_pressed = 0; break;

	case 0x36: rshift_pressed = 1; break;
	case 0xB6: rshift_pressed = 0; break;

	default:
// 		if (scan_code <= 0x80) {
// 			char c[2] = { keymap[scan_code][lshift_pressed || rshift_pressed], 0};
// 			dbg_put_str(c);
// 		}
// 		else if (scan_code >= 0x80 && scan_code < 0xE0)
// 			;//char c = keymap[scan_code - 0x80][lshift_pressed || rshift_pressed];
		break;
	}
}

void kb_initialize(u8 port) {
	dbg_puts("\nInitializing keyboard...\n");
}

