#include <debug_io.h>
#include <asm.h>

#include <drivers/pic.h>

#define ICW1_ICW4		0x01		// ICW4 (not) needed
#define ICW1_SINGLE		0x02		// Single (cascade) mode
#define ICW1_INTERVAL4	0x04		// Call address interval 4 (8)
#define ICW1_LEVEL		0x08		// Level triggered (edge) mode
#define ICW1_INIT		0x10		// Initialization - required!

#define ICW4_8086		0x01		// 8086/88 (MCS-80/85) mode
#define ICW4_AUTO		0x02		// Auto (normal) EOI
#define ICW4_BUF_SLAVE	0x08		// Buffered mode/slave
#define ICW4_BUF_MASTER	0x0C		// Buffered mode/master
#define ICW4_SFNM		0x10		// Special fully nested (not)

#define PIC_EOI 0x20

void pic_initialize(void) {
	dbg_printf("\nRemapping PIC...\n");

	u8 mask1 = inb(PIC1_DATA);
	u8 mask2 = inb(PIC2_DATA);

	outb(PIC1_CMD, ICW1_INIT | ICW1_ICW4);
	outb(PIC2_CMD, ICW1_INIT | ICW1_ICW4);

	outb(PIC1_DATA, PIC_IRQ_OFFS);
	outb(PIC2_DATA, PIC_IRQ_OFFS + 8);

	outb(PIC1_DATA, 4);
	outb(PIC2_DATA, 2);

	outb(PIC1_DATA, ICW4_8086);
	outb(PIC2_DATA, ICW4_8086);
}

static
u16 pic_get_irq_port(u8* irq) {
	u16 port = PIC1_DATA;

	if (*irq > 8) {
		port = PIC2_DATA;
		*irq -= 8;
	}
	return port;
}

void pic_mask_irq(u8 irq) {
	u16 port = pic_get_irq_port(&irq);
	u8 mask = inb(port);
	outb(port, mask | (1 << irq));
}

void pic_unmask_irq(u8 irq) {
	u16 port = pic_get_irq_port(&irq);
	u8 mask = inb(port);
	outb(port, mask & ~(1 << irq));
}

void pic_mask_all(void) {
	outb(PIC1_DATA, 0xFF);
	outb(PIC2_DATA, 0xFF);
}

void pic_unmask_all(void) {
	outb(PIC1_DATA, 0);
	outb(PIC2_DATA, 0);
}

void pic_eoi(u8 irq) {
	if (irq >= 8)
		outb(PIC2_CMD, PIC_EOI);
	outb(PIC1_CMD, PIC_EOI);
}
