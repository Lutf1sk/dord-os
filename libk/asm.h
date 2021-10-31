#ifndef ASM_H
#define ASM_H

#include "common.h"

#define EFLAGS_IF 0x0200
#define EFLAGS_DF 0x0400

static INLINE
void outb(u16 port, u8 val) {
	__asm__ volatile ("outb %0, %1\n" : : "Nd"(port), "a"(val));
}

static INLINE
void outw(u16 port, u16 val) {
	__asm__ volatile ("outw %0, %1\n" : : "Nd"(port), "a"(val));
}

static INLINE
void outl(u16 port, u32 val) {
	__asm__ volatile ("outd %0, %1\n" : : "Nd"(port), "a"(val));
}

static INLINE
u8 inb(u16 port) {
	u8 res;
	__asm__ volatile ("inb %0, %1\n" : "=a"(res) : "Nd"(port));
	return res;
}

static INLINE
u16 inw(u16 port) {
	u16 res;
	__asm__ volatile ("inw %0, %1\n" : "=a"(res) : "Nd"(port));
	return res;
}

static INLINE
u32 inl(u16 port) {
	u32 res;
	__asm__ volatile ("ind %0, %1\n" : "=a"(res) : "Nd"(port));
	return res;
}

static INLINE
void io_wait(void) {
	__asm__ volatile ( "outb 0x80, %%al" : : "a"(0) );
}


static INLINE
void hlt(void) { __asm__ volatile ("hlt\n"); }

static INLINE
void cli(void) { __asm__ volatile ("cli\n"); }

static INLINE
void sti(void) { __asm__ volatile ("sti\n"); }


static INLINE
void lidt(void* idt_addr) {
	__asm__ volatile ("lidt [%0]" : : "r"(idt_addr));
}

static INLINE
void lgdt(void* gdt_addr) {
	__asm__ volatile ("lgdt [%0]" : : "r"(gdt_addr));
}

static INLINE
u32 getf(void) {// Get current interrupt flag
	u32 ret;
	__asm__ volatile ("pushfd\npop %0\n\n" : "=Nd"(ret));
	return ret;
}

#endif

