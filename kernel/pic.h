#ifndef PIC_H
#define PIC_H

#include "common.h"

#define PIC1		0x20		/* IO base address for master PIC */
#define PIC2		0xA0		/* IO base address for slave PIC */
#define PIC1_CMD	PIC1
#define PIC1_DATA	(PIC1+1)
#define PIC2_CMD	PIC2
#define PIC2_DATA	(PIC2+1)

#define PIC_IRQ_OFFS 0x20

void pic_initialize(void);

void pic_mask_irq(u8 irq);
void pic_unmask_irq(u8 irq);

void pic_eoi(u8 irq);

#endif
