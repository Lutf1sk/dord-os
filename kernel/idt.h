#ifndef IDT_H
#define IDT_H

#include "common.h"

// This structure needs to be rewritten, since
// C bitfields are garbage and the order of
// the members is technically undefined
typedef
struct PACKED idt_attr {
	u8 gate_type : 4;
	u8 storage_seg : 1;
	u8 privilege : 2;
	u8 present : 1;
} idt_attr_t;


typedef
struct PACKED idt {
	u16 offset_low;
	u16 selector;
	u8 zero;
	idt_attr_t type_attr;
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
