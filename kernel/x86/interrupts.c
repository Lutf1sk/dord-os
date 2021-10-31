#include <asm.h>
#include <debug_io.h>
#include <proc.h>

#include <x86/interrupts.h>
#include <x86/exceptions.h>

#include <drivers/pic.h>

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

typedef
struct regs {
	usz a;
	usz c;
	usz d;
	usz b;
} regs_t;

usz sys_handler(regs_t* regs) {
	switch (regs->a) {
	case 1: // Print
		dbg_write(null, (void*)regs->c, regs->d);
		return 1;

	case 2: // Yield
		proc_yield();
		return 1;

	default:
		return 0;
	}
}

void interrupt_handler(u8 intr) {
	// Interrupt handlers
	if (is_pic_irq(intr)) {
		u8 irq = intr - PIC_IRQ_OFFS;

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

		case 7:
			dbg_puts("Spurious interrupt\n");
			pic_eoi(irq);
			hang();
			return; // Return without calling proc_schedule

		default:
			dbg_printf("Unknown interrupt %ud\n", irq);
			break;
		}
		pic_eoi(irq);

		u32 flags = proc_lock();
		proc_schedule();
		proc_release(flags);
	}
	// Exception handlers
	else if (is_exception(intr)) {
		switch (intr) {
		case EXCEPT_DIV_BY_0: dbg_puts("Divide by 0\n"); break;
		case EXCEPT_INVAL_OP: dbg_puts("Invalid opcode\n"); break;
		case EXCEPT_INVAL_TSS: dbg_puts("Invalid TSS\n"); break;
		case EXCEPT_GENERAL_PROTECTION_FAULT: dbg_puts("General protection fault\n"); break;
		case EXCEPT_PAGE_FAULT: dbg_puts("Page fault\n"); break;
		default:
			dbg_printf("Unknown exception %ud\n", intr);
			break;
		}
		hang();
	}
}
