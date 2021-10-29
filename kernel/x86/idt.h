#ifndef IDT_H
#define IDT_H

#include <common.h>

// This structure needs to be rewritten, since
// C bitfields are garbage and the order of
// the members is technically undefined

#define IDT_STORESEG	(1 << 4)
#define IDT_PRESENT		(1 << 7)

#define IDT_RING_0 0b00000000
#define IDT_RING_1 0b00100000
#define IDT_RING_2 0b01000000
#define IDT_RING_3 0b01100000

#define IDT_GTYPE_TASK32 0x05
#define IDT_GTYPE_INTR16 0x06
#define IDT_GTYPE_TRAP16 0x07
#define IDT_GTYPE_INTR32 0x0E
#define IDT_GTYPE_TRAP32 0x0F

typedef
struct PACKED idt {
	u16 offset_low;
	u16 selector;
	u8 zero;
	u8 attr;
	u16 offset_high;
} idt_t;


// Descriptor to be loaded with 'lidt' instruction
typedef
struct PACKED idt_desc {
	u16 size;
	u32 addr;
} idt_desc_t;

void idt_make_default(idt_t* idt);

#endif
