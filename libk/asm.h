#ifndef ASM_H
#define ASM_H

#include "common.h"

#define EFLAGS_IF 0x0200
#define EFLAGS_DF 0x0400

static INLINE
void outb(u16 port, u8 val) {
	asm volatile ("outb %0, %1\n" : : "Nd"(port), "a"(val));
}

static INLINE
void outw(u16 port, u16 val) {
	asm volatile ("outw %0, %1\n" : : "Nd"(port), "a"(val));
}

static INLINE
void outl(u16 port, u32 val) {
	asm volatile ("outd %0, %1\n" : : "Nd"(port), "a"(val));
}

static INLINE
u8 inb(u16 port) {
	u8 res;
	asm volatile ("inb %0, %1\n" : "=a"(res) : "Nd"(port));
	return res;
}

static INLINE
u16 inw(u16 port) {
	u16 res;
	asm volatile ("inw %0, %1\n" : "=a"(res) : "Nd"(port));
	return res;
}

static INLINE
u32 inl(u16 port) {
	u32 res;
	asm volatile ("ind %0, %1\n" : "=a"(res) : "Nd"(port));
	return res;
}

static INLINE
void io_wait(void) {
	asm volatile ( "outb 0x80, %%al" : : "a"(0) );
}


static INLINE
void hlt(void) { __asm__ volatile ("hlt\n"); }

static INLINE NOREORDER
void cli(void) { __asm__ volatile ("cli\n"); }

static INLINE NOREORDER
void sti(void) { __asm__ volatile ("sti\n"); }


static INLINE
void lidt(void* idt_addr) {
	asm volatile ("lidt [%0]" : : "r"(idt_addr));
}

static INLINE
void lgdt(void* gdt_addr) {
	asm volatile ("lgdt [%0]" : : "r"(gdt_addr));
}

static INLINE NOREORDER
u32 getf(void) {
	u32 ret;
	asm volatile ("pushfd\npop %0\n" : "=Nd"(ret));
	return ret;
}

static INLINE NOREORDER
void setf(u32 flags) {
	asm volatile ("push %0\npopfd\n" : : "Nd"(flags));
}

#endif

