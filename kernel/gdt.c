#include "gdt.h"

#include "memory.h"

void gdt_make_flat(gdt_t* gdt) {
	mset8(gdt, 0, sizeof(gdt_t) * 3);

	gdt[1].access = 0b10011010;
	gdt[1].base_high = 0x0;
	gdt[1].base_low = 0x0;
	gdt[1].base_mid = 0x0;
	gdt[1].flags = 0b11001111;
	gdt[1].limit_low = 0xFFFF;

	gdt[2].access = 0b10010010;
	gdt[2].base_high = 0x0;
	gdt[2].base_low = 0x0;
	gdt[2].base_mid = 0x0;
	gdt[2].flags = 0b11001111;
	gdt[2].limit_low = 0xFFFF;
}
