#include "pc_speaker.h"

#include "asm.h"

#define PCSPK_CMD 0x61

#define PCSPK_CONNECT_PIT2 0b11

void pcspk_connect_pit(void) {
	outb(PCSPK_CMD, PCSPK_CONNECT_PIT2);
}

void pcspk_disconnect_pit(void) {
	outb(PCSPK_CMD, ~PCSPK_CONNECT_PIT2);
}
