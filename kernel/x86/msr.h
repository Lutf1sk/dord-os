#ifndef MSR_H
#define MSR_H 1

#include <common.h>

#define MSR_APIC_BASE	0x1B
#define MSR_APIC_ENABLE	0x800

static INLINE
void rdmsr(u32 msr, u32* eax, u32* edx) {
	asm volatile ("rdmsr" : "=a"(*eax), "=d"(*edx) : "c"(msr));
}

static INLINE
void wrmsr(u32 msr, u32 eax, u32 edx) {
	asm volatile ("wrmsr" : : "a"(eax), "d"(edx), "c"(msr));
}

#endif
