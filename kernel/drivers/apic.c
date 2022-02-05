#include <debug_io.h>

#include <drivers/apic.h>

#define LAPIC_REG_ID		0x20
#define LAPIC_REG_VERSION	0x30
#define LAPIC_REG_TPR		0x80
#define LAPIC_REG_APR		0x90
#define LAPIC_REG_PPR		0xA0
#define LAPIC_REG_EOI		0xB0
#define LAPIC_REG_RRD		0xC0
#define LAPIC_REG_LDR		0xD0
#define LAPIC_REG_DFR		0xE0
#define LAPIC_REG_SPUR		0xF0

#define IOAPIC_REG_ID		0x00
#define IOAPIC_REG_VERSION	0x01
#define IOAPIC_REG_ARBPRI	0x02
#define IOAPIC_REG_REDIR(n, o)	(0x10 + ((n)<<1) + (o))

u8 volatile* lapic_base = NULL;
u32 volatile* ioapic_base = NULL;

static
void lapic_write(u16 reg, u32 val) {
	*(u32*)(lapic_base + reg) = val;
}

static
u32 lapic_read(u16 reg) {
	return *(u32*)(lapic_base + reg);
}

static
void ioapic_write(u8 reg, u32 val) {
	ioapic_base[0] = reg;
	ioapic_base[4] = val;
}

static
u32 ioapic_read(u8 reg) {
	ioapic_base[0] = reg;
	return ioapic_base[4];
}

void apic_initialize(void* lbase, void* iobase) {
	dbg_printf("\nInitializing APIC...\n");

	lapic_base = lbase;
	ioapic_base = iobase;

	u32 spur = lapic_read(LAPIC_REG_SPUR);
	lapic_write(LAPIC_REG_SPUR, spur | 0x1FF);

	lapic_write(LAPIC_REG_DFR, 0xF0000000);
	lapic_write(LAPIC_REG_LDR, 0xFF000000);
	lapic_write(LAPIC_REG_TPR, 0);

	for (usz i = 0; i < 24; ++i) {
		u32 redir_lo = (APIC_IRQ_OFFS + i);

		ioapic_write(IOAPIC_REG_REDIR(i, 0), redir_lo);
		ioapic_write(IOAPIC_REG_REDIR(i, 1), 0);
	}
}

void apic_eoi(u8 irq) {
	lapic_write(LAPIC_REG_EOI, 0);
}

