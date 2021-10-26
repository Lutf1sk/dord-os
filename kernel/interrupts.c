#include "interrupts.h"

#include "pic.h"
#include "asm.h"
#include "debug_io.h"
#include "exceptions.h"

extern void pit_handle_interrupt(void);
extern void kb_handle_interrupt(void);
extern void mouse_handle_interrupt(void);

static u8 is_pic_irq(u8 irq) {
	return irq >= PIC_IRQ_OFFS && irq < PIC_IRQ_OFFS + 16;
}

static u8 is_exception(u8 irq) {
	return irq < PIC_IRQ_OFFS;
}

static void NORETURN hang(void) {
	for(;;) {
		cli();
		hlt();
	}
}

void irq_handler(i8 irq) {
	// Interrupt handlers
	if (is_pic_irq(irq)) {
		irq -= PIC_IRQ_OFFS;

		switch (irq) {
		case IRQ_PIT:
			pit_handle_interrupt();
			break;

		case IRQ_KB:
			kb_handle_interrupt();
			break;

		case IRQ_ATA1:
			dbg_puts("ATA1 interrupt received\n");
			break;

		case IRQ_ATA2:
			dbg_puts("ATA2 interrupt received\n");
			break;

		case IRQ_MOUSE:
			mouse_handle_interrupt();
			break;

		default:
			dbg_printf("Unknown interrupt %i received\n", irq);
			break;
		}

		pic_eoi(irq);
	}
	// Exception handlers
	else if (is_exception(irq)) {
		switch (irq) {
		case EXCEPT_DIV_BY_0: dbg_puts("Divide by 0 received\n"); break;
		case EXCEPT_INVAL_OP: dbg_puts("Invalid opcode received\n"); break;
		case EXCEPT_INVAL_TSS: dbg_puts("Invalid TSS received\n"); break;
		case EXCEPT_GENERAL_PROTECTION_FAULT: dbg_puts("General protection fault received\n"); break;
		case EXCEPT_PAGE_FAULT: dbg_puts("Page fault received\n"); break;
		default:
			dbg_printf("Unknown exception %i received\n", irq);
			break;
		}
		hang();
	}
}
