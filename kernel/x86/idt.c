#include <memory.h>

#include <x86/idt.h>

void idt_make_default(idt_t* idt) {
	idt_t entry;
	mset8(&entry, 0, sizeof(entry));

#define ADD_TRAP(n) \
	extern void isr##n(void);\
 \
	entry.offset_low = (u32)(usz)isr##n; \
	entry.offset_high = (u32)(usz)isr##n >> 16; \
	entry.selector = 0x8; \
	entry.attr = IDT_GTYPE_INTR32; \
	entry.attr |= IDT_PRESENT; \
	entry.attr |= IDT_RING_0; \
	idt[n] = entry;

#define ADD_INTERRUPT(n) \
	extern void isr##n(void);\
 \
	entry.offset_low = (u32)(usz)isr##n; \
	entry.offset_high = (u32)(usz)isr##n >> 16; \
	entry.selector = 0x8; \
	entry.attr = IDT_GTYPE_INTR32; \
	entry.attr |= IDT_PRESENT; \
	entry.attr |= IDT_RING_0; \
	idt[n] = entry;

	// Add exception handlers
	ADD_INTERRUPT(0);
	ADD_TRAP(1);
	ADD_INTERRUPT(2);
	ADD_TRAP(3);
	ADD_TRAP(4);
	ADD_INTERRUPT(5);
	ADD_INTERRUPT(6);
	ADD_INTERRUPT(7);
	ADD_INTERRUPT(8);
	ADD_INTERRUPT(9);
	ADD_INTERRUPT(10);
	ADD_INTERRUPT(11);
	ADD_INTERRUPT(12);
	ADD_INTERRUPT(13);
	ADD_INTERRUPT(14);
	ADD_INTERRUPT(15);
	ADD_INTERRUPT(16);
	ADD_INTERRUPT(17);
	ADD_INTERRUPT(18);
	ADD_INTERRUPT(19);
	ADD_INTERRUPT(20);
	ADD_INTERRUPT(21);
	ADD_INTERRUPT(22);
	ADD_INTERRUPT(23);
	ADD_INTERRUPT(24);
	ADD_INTERRUPT(25);
	ADD_INTERRUPT(26);
	ADD_INTERRUPT(27);
	ADD_INTERRUPT(28);
	ADD_INTERRUPT(29);
	ADD_INTERRUPT(30);
	ADD_INTERRUPT(31);

	// Add PIC IRQ handlers
	ADD_INTERRUPT(32);
	ADD_INTERRUPT(33);
	ADD_INTERRUPT(34);
	ADD_INTERRUPT(35);
	ADD_INTERRUPT(36);
	ADD_INTERRUPT(37);
	ADD_INTERRUPT(38);
	ADD_INTERRUPT(39);
	ADD_INTERRUPT(40);
	ADD_INTERRUPT(41);
	ADD_INTERRUPT(42);
	ADD_INTERRUPT(43);
	ADD_INTERRUPT(44);
	ADD_INTERRUPT(45);
	ADD_INTERRUPT(46);
	ADD_INTERRUPT(47);

	// Add APIC IRQ handlers
	ADD_INTERRUPT(64);
	ADD_INTERRUPT(65);
	ADD_INTERRUPT(66);
	ADD_INTERRUPT(67);
	ADD_INTERRUPT(68);
	ADD_INTERRUPT(69);
	ADD_INTERRUPT(70);
	ADD_INTERRUPT(71);
	ADD_INTERRUPT(72);
	ADD_INTERRUPT(73);
	ADD_INTERRUPT(74);
	ADD_INTERRUPT(75);
	ADD_INTERRUPT(76);
	ADD_INTERRUPT(77);
	ADD_INTERRUPT(78);
	ADD_INTERRUPT(79);
	ADD_INTERRUPT(80);
	ADD_INTERRUPT(81);
	ADD_INTERRUPT(82);
	ADD_INTERRUPT(83);
	ADD_INTERRUPT(84);
	ADD_INTERRUPT(85);
	ADD_INTERRUPT(86);

	extern void isr_sys(void);

	entry.offset_low = (u32)(usz)isr_sys;
	entry.offset_high = (u32)(usz)isr_sys >> 16;
	entry.selector = 0x8;
	entry.attr = IDT_GTYPE_INTR32;
	entry.attr |= IDT_PRESENT;
	entry.attr |= IDT_RING_0;
	idt[0xAE] = entry;
}
