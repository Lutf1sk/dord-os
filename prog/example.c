#include <common.h>

u32 syscall(u32 a, u32 c, u32 d) {
	u32 ret;
	__asm__ volatile ("int 0xAE\n" : "=a"(ret) : "a"(a), "c"(c), "d"(d));
	return ret;
}

int start(void) {
	syscall(1, (u32)"Skjut mig\n", 10);
	return -69;
}

