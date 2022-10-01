#ifndef SYSCALL_H
#define SYSCALL_H 1

#include <common.h>

#define SYS_PRINT	0
#define SYS_YIELD	1
#define SYS_FREAD	2
#define SYS_FWRITE	3

static INLINE
u32 syscall(u32 a, u32 c, u32 d) {
	u32 ret;
	__asm__ volatile ("int 0xAE\n" : "=a"(ret) : "a"(a), "c"(c), "d"(d));
	return ret;
}

#endif
