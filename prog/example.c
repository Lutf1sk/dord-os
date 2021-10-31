#include <syscall.h>

usz (*dbg_puts)() = (void*)0x100060;

void print(char* str, u32 len) {
	syscall(1, (u32)str, len);
}

void yield(void) {
	syscall(2, 0, 0);
}

int start(void) {
	char buf[] =  "ASDF\n";
	print("Entered process\n", 16);
	while (1) {
// 		yield();
//		dbg_puts("Child\n", 6);
	}
	return -69;
}

