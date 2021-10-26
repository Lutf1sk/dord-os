#ifndef ASM_H
#define ASM_H

#include "common.h"

static inline INLINE
void outb(u16 port, u8 val) {
	__asm__ volatile ("outb %0, %1\n" : : "Nd"(port), "a"(val));
}

static inline INLINE
void outw(u16 port, u16 val) {
	__asm__ volatile ("outw %0, %1\n" : : "Nd"(port), "a"(val));
}

static inline INLINE
void outl(u16 port, u32 val) {
	__asm__ volatile ("outd %0, %1\n" : : "Nd"(port), "a"(val));
}

static inline INLINE
u8 inb(u16 port) {
	u8 res;
	__asm__ volatile ("inb %0, %1\n" : "=a"(res) : "Nd"(port));
	return res;
}

static inline INLINE
u16 inw(u16 port) {
	u16 res;
	__asm__ volatile ("inw %0, %1\n" : "=a"(res) : "Nd"(port));
	return res;
}

static inline INLINE
u32 inl(u16 port) {
	u32 res;
	__asm__ volatile ("ind %0, %1\n" : "=a"(res) : "Nd"(port));
	return res;
}

static inline INLINE
void io_wait(void) {
	__asm__ volatile ( "outb 0x80, %%al" : : "a"(0) );
}


static inline INLINE
void hlt(void) { __asm__ volatile ("hlt\n"); }

static inline INLINE
void cli(void) { __asm__ volatile ("cli\n"); }

static inline INLINE
void sti(void) { __asm__ volatile ("sti\n"); }

#endif

