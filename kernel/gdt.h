#ifndef GDT_H
#define GDT_H

#include "common.h"

typedef
struct PACKED gdt {
	u16 limit_low;
	u16 base_low;
	u8 base_mid;
	u8 access;
	u8 flags;
	u8 base_high;
} gdt_t;


// Descriptor to be loaded with 'lgdt' instruction
typedef
struct PACKED gdt_desc {
	u16 size;
	u32 addr;
} gdt_desc_t;

void gdt_make_flat(gdt_t* gdt);

#endif
