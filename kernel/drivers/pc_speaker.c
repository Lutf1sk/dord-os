#include <asm.h>

#include <drivers/pc_speaker.h>

void pcspk_connect_pit(void) {
	outb(PCSPK_CMD, PCSPK_CONNECT_PIT2);
}

void pcspk_disconnect_pit(void) {
	outb(PCSPK_CMD, ~PCSPK_CONNECT_PIT2);
}
