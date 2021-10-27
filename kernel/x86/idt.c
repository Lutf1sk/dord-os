#include <memory.h>

#include <x86/idt.h>

void idt_make_default(idt_t* idt) {
	idt_t entry;
	mset8(&entry, 0, sizeof(entry));

#define ADD_TRAP(n) \
	extern void irq##n##_handler_asm(void);\
 \
	entry.offset_low = (u32)(usz)irq##n##_handler_asm; \
	entry.offset_high = (u32)(usz)irq##n##_handler_asm >> 16; \
	entry.selector = 0x8; \
	entry.type_attr.gate_type = 0xF; \
	entry.type_attr.present = 1; \
	entry.type_attr.privilege = 0; \
	entry.type_attr.storage_seg = 0; \
	idt[n] = entry;

#define ADD_INTERRUPT(n) \
	extern void irq##n##_handler_asm(void);\
 \
	entry.offset_low = (u32)(usz)irq##n##_handler_asm; \
	entry.offset_high = (u32)(usz)irq##n##_handler_asm >> 16; \
	entry.selector = 0x8; \
	entry.type_attr.gate_type = 0xE; \
	entry.type_attr.present = 1; \
	entry.type_attr.privilege = 0; \
	entry.type_attr.storage_seg = 0; \
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

	// Add IRQ handlers
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
}
