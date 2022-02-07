#include <asm.h>
#include <debug_io.h>

#include <drivers/keyboard.h>
#include <drivers/ps2.h>

u32 kb_keymap1[256][2] = {
	{ 0, 0 },
	{ KEY_F9,	KEY_F9 },
	{ 0, 0 },
	{ KEY_F5,	KEY_F5 },
	{ KEY_F3,	KEY_F3 },
	{ KEY_F1,	KEY_F1 },
	{ KEY_F2,	KEY_F2 },
	{ KEY_F12,	KEY_F12 },
	{ 0, 0 },
	{ KEY_F10,	KEY_F10 },
	{ KEY_F8,	KEY_F8 },
	{ KEY_F6,	KEY_F6 },
	{ KEY_F4,	KEY_F4 },

	{ '\t',	'\t' },
	{ '`',	'~' },

	{ 0, 0 },
	{ 0, 0 },

	{ KEY_LALT,		KEY_LALT },
	{ KEY_LSHIFT,	KEY_LSHIFT },
	{ 0, 0 },
	{ KEY_LCTRL,	KEY_LCTRL },

	{ 'q',	'Q' },
	{ '1',	'!' },

	{ 0, 0 },
	{ 0, 0 },
	{ 0, 0 },

	{ 'z',	'Z' },
	{ 's',	'S' },
	{ 'a',	'A' },
	{ 'w',	'W' },
	{ '2',	'@' },

	{ 0, 0 },
	{ 0, 0 },

	{ 'c',	'C' },
	{ 'x',	'X' },
	{ 'd',	'D' },
	{ 'e',	'E' },
	{ '4',	'$' },
	{ '3',	'#' },

	{ 0, 0 },
	{ 0, 0 },

	{ ' ',	' ' },
	{ 'v',	'V' },
	{ 'f',	'F' },
	{ 't',	'T' },
	{ 'r',	'R' },
	{ '5',	'%' },

	{ 0, 0 },
	{ 0, 0 },

	{ 'n',	'N' },
	{ 'b',	'B' },
	{ 'h',	'H' },
	{ 'g',	'G' },
	{ 'y',	'Y' },
	{ '6',	'^' },

	{ 0, 0 },
	{ 0, 0 },
	{ 0, 0 },

	{ 'm',	'M' },
	{ 'j',	'J' },
	{ 'u',	'U' },
	{ '7',	'&' },
	{ '8',	'*' },

	{ 0, 0 },
	{ 0, 0 },

	{ ',',	'<' },
	{ 'k',	'K' },
	{ 'i',	'I' },
	{ 'o',	'O' },
	{ '0',	')' },
	{ '9',	'(' },

	{ 0, 0 },
	{ 0, 0 },

	{ '.',	'>' },
	{ '/',	'?' },
	{ 'l',	'L' },
	{ ';',	':' },
	{ 'p',	'P' },
	{ '-',	'_' },

	{ 0, 0 },
	{ 0, 0 },
	{ 0, 0 },

	{ '\'',	'"' },

	{ 0, 0 },

	{ '[',	'{' },
	{ '=',	'+' },

	{ 0, 0 },
	{ 0, 0 },

	{ KEY_CAPSLOCK, KEY_CAPSLOCK },
	{ KEY_RSHIFT, KEY_RSHIFT },
	{ '\n',	'\n' },
	{ ']',	'}' },

	{ 0, 0 },

	{ '\\',	'|' },

	{ 0, 0 },
	{ 0, 0 },
	{ 0, 0 },
	{ 0, 0 },
	{ 0, 0 },
	{ 0, 0 },
	{ 0, 0 },
	{ 0, 0 },

	{ '\b',	'\b' },

	{ 0, 0 },
	{ 0, 0 },

	{ 0, 0 }, // Numpad 1

	{ 0, 0 },

	{ 0, 0 }, // Numpad 4
	{ 0, 0 }, // Numpad 7

	{ 0, 0 },
	{ 0, 0 },
	{ 0, 0 },

	{ 0, 0 }, // Numpad 0
	{ 0, 0 }, // Numpad .
	{ 0, 0 }, // Numpad 2
	{ 0, 0 }, // Numpad 5
	{ 0, 0 }, // Numpad 6
	{ 0, 0 }, // Numpad 8
	{ KEY_ESC,		KEY_ESC },
	{ KEY_NUMLOCK,	KEY_NUMLOCK },
	{ KEY_F11,		KEY_F11 },
	{ 0, 0 }, // Numpad +
	{ 0, 0 }, // Numpad 3
	{ 0, 0 }, // Numpad -
	{ 0, 0 }, // Numpad *
	{ 0, 0 }, // Numpad 9
	{ KEY_SCROLLLOCK, KEY_SCROLLLOCK },

	{ 0, 0 },
	{ 0, 0 },
	{ 0, 0 },
	{ 0, 0 },

	{ KEY_F7, KEY_F7 },
};

u32 kb_keymap2[256][2] = {
	[0x10] = { 0, 0 }, // WWW Search
	[0x11] = { KEY_RALT, KEY_RALT },
	[0x14] = { KEY_RCTRL, KEY_RCTRL },
	[0x15] = { KEY_PREV, KEY_PREV },
	[0x18] = { 0, 0 }, // WWW Favourites
	[0x1F] = { KEY_LMETA, KEY_LMETA },
	[0x20] = { 0, 0 }, // WWW Refresh
	[0x21] = { KEY_VOL_DOWN, KEY_VOL_DOWN },
	[0x23] = { KEY_MUTE, KEY_MUTE },
	[0x27] = { KEY_RMETA, KEY_RMETA },
	[0x28] = { 0, 0 }, // WWW Stop
	[0x2B] = { 0, 0 }, // Calculator
	[0x2F] = { 0, 0 }, // Apps
	[0x30] = { 0, 0 }, // WWW Forward
	[0x32] = { KEY_VOL_UP, KEY_VOL_UP },
	[0x34] = { KEY_PLAY_PAUSE, KEY_PLAY_PAUSE },
	[0x37] = { 0, 0 }, // Power
	[0x38] = { 0, 0 }, // WWW Back
	[0x3A] = { 0, 0 }, // WWW Home
	[0x3B] = { KEY_STOP, KEY_STOP },
	[0x3F] = { 0, 0 }, // Sleep
	[0x40] = { 0, 0 }, // My computer
	[0x48] = { 0, 0 }, // E-mail
	[0x4A] = { 0, 0 }, // Numpad /
	[0x4D] = { KEY_NEXT, KEY_NEXT },
	[0x50] = { 0, 0 }, // Media select
	[0x5A] = { 0, 0 }, // Numpad enter
	[0x5E] = { 0, 0 }, // Wake
	[0x69] = { KEY_END, KEY_END },
	[0x6B] = { KEY_LEFT, KEY_LEFT },
	[0x6C] = { KEY_HOME, KEY_HOME },
	[0x70] = { KEY_INSERT, KEY_INSERT },
	[0x71] = { KEY_DELETE, KEY_DELETE },
	[0x72] = { KEY_DOWN, KEY_DOWN },
	[0x74] = { KEY_RIGHT, KEY_RIGHT },
	[0x75] = { KEY_UP, KEY_UP },
	[0x7A] = { KEY_PAGEDOWN, KEY_PAGEDOWN },
	[0x7D] = { KEY_PAGEUP, KEY_PAGEUP },
};

static u8 scan_buf[16];
static usz scan_len = 0;

void kb_handle_interrupt(void) {
	u8 scan_code = inb(PS2_DATA);

	static u8 shift_pressed = 0;

	scan_buf[scan_len++] = scan_code;

	if (scan_code != 0xF0 && scan_code != 0xE0 && scan_code != 0xE1) {
		u8* it = scan_buf;
		u8 press = 1;

		u32 (*keymap)[2] = kb_keymap1;

		if (*it == 0xE0) {
			keymap = kb_keymap2;
			++it;
		}

		if (*it == 0xF0) {
			press = 0;
			++it;
		}

		u32 c = keymap[*it][shift_pressed];

		if (c == KEY_LSHIFT || c == KEY_RSHIFT)
			shift_pressed = press;

		if (!c) {
			dbg_printf("Unknown scancode ");
			for (usz i = 0; i < scan_len; ++i)
				dbg_printf(" 0x%hd", scan_buf[i]);
			dbg_printf("\n");
		}
		else if (press && c < 256)
			dbg_printf("%c", c);

		scan_len = 0;
	}
}

void kb_initialize(u8 port) {
	dbg_puts("\nInitializing keyboard...\n");
}

