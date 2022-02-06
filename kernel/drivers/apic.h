#ifndef APIC_H
#define APIC_H 1

#define APIC_IO_BASE_DEFAULT 0xFEC00000

#define APIC_IRQ_OFFS 0x40

#include <common.h>

void apic_initialize(void* lbase, void* iobase, u8* irq_mappings);
void apic_start_core(u8 core);

void apic_eoi(u8 irq);

#endif
