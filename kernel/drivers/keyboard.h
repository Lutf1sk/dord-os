#ifndef KEYBOARD_H
#define KEYBOARD_H

#include <common.h>

typedef
enum keycode {
	KEY_INVAL	= 0,

	KEY_LALT	= 256,
	KEY_RALT,

	KEY_LSHIFT,
	KEY_RSHIFT,

	KEY_LCTRL,
	KEY_RCTRL,

	KEY_LMETA,
	KEY_RMETA,

	KEY_CAPSLOCK,
	KEY_NUMLOCK,
	KEY_SCROLLLOCK,

	KEY_PAGEUP,
	KEY_PAGEDOWN,

	KEY_DELETE,
	KEY_INSERT,
	KEY_HOME,
	KEY_END,

	KEY_UP,
	KEY_DOWN,
	KEY_LEFT,
	KEY_RIGHT,

	KEY_ESC,

	KEY_F1,
	KEY_F2,
	KEY_F3,
	KEY_F4,
	KEY_F5,
	KEY_F6,
	KEY_F7,
	KEY_F8,
	KEY_F9,
	KEY_F10,
	KEY_F11,
	KEY_F12,

	KEY_PREV,
	KEY_NEXT,
	KEY_PLAY_PAUSE,
	KEY_STOP,
	KEY_MUTE,
	KEY_VOL_DOWN,
	KEY_VOL_UP,

	KEY_MAX,
} keycode_t;

void kb_handle_interrupt(void);

void kb_initialize(u8 port);

#endif
