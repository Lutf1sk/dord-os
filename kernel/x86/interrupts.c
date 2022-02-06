#include <asm.h>
#include <debug_io.h>
#include <proc.h>

#include <x86/cpuid.h>

#include <x86/interrupts.h>
#include <x86/exceptions.h>

#include <drivers/pic.h>
#include <drivers/apic.h>

u8 interrupt_mode = INTM_PIC;

extern void pit_handle_interrupt(void);
extern void kb_handle_interrupt(void);
extern void mouse_handle_interrupt(void);

static u8 is_pic_irq(u8 irq) {
	return irq >= PIC_IRQ_OFFS && irq < PIC_IRQ_OFFS + 16;
}

static u8 is_apic_irq(u8 irq) {
	return irq >= APIC_IRQ_OFFS && irq < APIC_IRQ_OFFS + 24;
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
	if (intr == 0xFF)
		goto spurious;

	u8 apic = is_apic_irq(intr);
	u8 pic = is_pic_irq(intr);
	u8 is_irq = apic || pic;

	u8 irq = intr;
	if (apic)
		irq -= APIC_IRQ_OFFS;
	else if (pic)
		irq -= PIC_IRQ_OFFS;

// 	dbg_printf("[CPU%ud] I%ud\n", cpu_lapic_id(), irq);

	// IRQ handlers
	if (is_irq) {
		switch (irq) {
		case IRQ_PIT:
			pit_handle_interrupt();

			if (pic)
				pic_eoi(irq);
			else if (apic)
				apic_eoi(irq);

			volatile u32 flags = proc_lock();
			proc_schedule();
			proc_release(flags);
			return;

		case IRQ_KB:
			dbg_printf("keyboard interrupt\n");
			kb_handle_interrupt();
			break;

		case IRQ_CASCADE:
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

		case 7: spurious:
			dbg_puts("Spurious interrupt\n");
			break;

		default:
			dbg_printf("Unknown interrupt %ud\n", irq);
			break;
		}

		if (pic)
			pic_eoi(irq);
		else if (apic)
			apic_eoi(irq);
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
