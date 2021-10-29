#include <memory.h>
#include <debug_io.h>
#include <asm.h>
#include <pmman.h>
#include <elf.h>

#include <x86/cpuid.h>
#include <x86/idt.h>
#include <x86/gdt.h>

#include <drivers/memmap.h>
#include <drivers/dfs.h>
#include <drivers/pic.h>
#include <drivers/pci.h>
#include <drivers/ide.h>
#include <drivers/pit.h>
#include <drivers/ps2.h>
#include <drivers/vbe.h>
#include <drivers/vga.h>
#include <drivers/mouse.h>
#include <drivers/keyboard.h>
#include <drivers/acpi.h>

// typedef
// struct proc {
// 	void* sp;
// 	usz[16] stack;
// } proc_t;
// 
// proc_t procs[16];
// usz proc_count;

NORETURN
void panic(const char* str) {
	dbg_printf(DBG_RED"KERNEL PANIC: %s\n", str);
	pit_sleep_msec(10);
	for (;;) {
		cli();
		hlt();
	}
}

gdt_t gdt[3];
gdt_desc_t gdt_desc;

idt_t idt[256] = {};
idt_desc_t idt_desc;

ide_drive_t* drives = 0;
u32 drive_count = 0;

extern void reload_segments_asm(void);

// The bootinf structure is filled by entry.asm,
// with information provided to it by the bootloader
typedef
struct PACKED bootinf {
	void* kernel_start;
	void* kernel_end;
	memmap_t* memmap;
	vbe_mib_t* mib;
	u16 memmap_entry_count;
	u8 boot_drive;
} bootinf_t;

extern bootinf_t* boot_info_ptr;

bootinf_t boot_info;
vbe_mib_t mib;
memmap_t* memmap;

void kernel_enter() {
	dbg_puts(DBG_CYN"\nENTERED KERNEL\n"DBG_RST);
	dbg_printf("CPU: '%s' %s\n", cpu_vendor_str(), cpu_brand_str());

	// Copy the important structures from the stage2 memory
	boot_info = *boot_info_ptr;
	mib = *boot_info.mib;

	// Initialize physical memory manager and mark reserved areas
	pmman_initialize(boot_info.memmap, boot_info.memmap_entry_count);

	pmman_map_t* pmkmap = &pmman_kernel_map;
	usz kernel_len = boot_info.kernel_end - boot_info.kernel_start;
	pmman_mark_range(pmkmap, pmman_to_block(pmkmap, boot_info.kernel_start), pmman_to_blocks(pmkmap, kernel_len)); // Mark kernel
	pmman_mark_range(pmkmap, 0, pmman_to_blocks(pmkmap, 0x7C00)); // Mark low BIOS
	// Mark the BIOS memory map to keep it from being overwritten
	pmman_mark_range(pmkmap, pmman_to_block(pmkmap, boot_info.memmap), pmman_to_blocks(pmkmap, sizeof(memmap_t) * boot_info.memmap_entry_count));

	// Write and load flat GDT
	gdt_make_flat(gdt);
	gdt_desc.size = sizeof(gdt);
	gdt_desc.addr = (usz)gdt;
	lgdt(&gdt_desc);
	reload_segments_asm();

	// Write and load default IDT
	idt_make_default(idt);
	idt_desc.size = sizeof(idt);
	idt_desc.addr = (u32)idt;
	lidt(&idt_desc);

	// Initialize ACPI
	acpi_initialize();

	// Remap PIC and unmask IRQs
	pic_initialize();
	for (usz i = 0; i < 16; ++i)
		pic_unmask_irq(i);

	sti(); // Enable interrupts

	// Initialize VGA framebuffer
	vga_initialize((void*)boot_info.mib->phys_base_addr, boot_info.mib->x_resolution, boot_info.mib->y_resolution);
	vga_clear(0x000000);

	// Initialize PIT
	//pcspk_connect_pit();
	pit_initialize();

	// Initialize PS/2 controller
	ps2_initialize();

	// Initialize any detected PS/2 devices
	kb_initialize(PS2_PORT1);
	if (ps2_p2 == PS2_DEV_GENERIC_MOUSE)
		mouse_initialize(PS2_PORT2);
	ps2_enable_irqs();

	// Enumerate and initialize PCI devices
	u32 pci_device_count = 0;
	pci_dev_t* pci_devices = pci_enumerate(&pci_device_count);
	u8 ide_found = 0;
	for (int i = 0; i < pci_device_count; ++i) {
		pci_dev_t* dev = &pci_devices[i];

		if (dev->reg_data[PCI_HC_HEADER_TYPE] == PCI_HEADER_TYPE_0 &&
			dev->reg_data[PCI_HC_CLASS]	== 0x01 &&
			dev->reg_data[PCI_HC_SUBCLASS] == 0x01)
		{
			drives = ide_initialize(&drive_count,
				pci_reg32(dev, PCI_H0_BAR0), pci_reg32(dev, PCI_H0_BAR1),
				pci_reg32(dev, PCI_H0_BAR2), pci_reg32(dev, PCI_H0_BAR3),
				pci_reg32(dev, PCI_H0_BAR4)
			);
			ide_found = 1;
		}
		if (dev->reg_data[PCI_HC_HEADER_TYPE] == PCI_HEADER_TYPE_0 &&
			dev->reg_data[PCI_HC_CLASS]	== 0x0C &&
			dev->reg_data[PCI_HC_SUBCLASS] == 0x03)
		{
			// USB Controller
		}
	}
	if (!ide_found)
		panic("No usable IDE Controller found");

	// String tables for IDE drive types/ports
	static char* drive_type[4] = { "ATA", "ATAPI", "SATA", "SATAPI" };
	static char* drive_bus[2] = { "Master", "Slave" };
	static char* drive_channel[2] = { "Primary", "Secondary" };

	u8* example = null;

	for (usz i = 0; i < drive_count; ++i) {
		if (drives[i].type != IDE_ATA) // Only search ATA drives (for now)
			continue;

		dbg_printf("\n'%s' [%s %s %s] (%ud sectors):\n", drives[i].name,
				drive_type[drives[i].type], drive_channel[drives[i].channel], drive_bus[drives[i].bus], drives[i].size);

		dfs_it_t it = dfs_it_begin(&drives[i]);
		while (dfs_iterate(&it)) {
			dbg_printf(DBG_GRY"File: '%s' (%ud Bytes)\n"DBG_RST, it.name, it.sectors * 512);
			if (strneq(it.name, "example.bin", 32)) {
				example = pmman_alloc(pmkmap, it.sectors * 512);
				dfs_read(example, &it);
			}
		}
	}

	elf32_fh_t* fh = (elf32_fh_t*)example;

	elf32_ph_t* phs = (elf32_ph_t*)(example + fh->ph_offset);

	for (usz i = 0; i < fh->sh_count; ++i) {
		elf32_ph_t* ph = &phs[i];

		if (ph->type == ELF_PH_TYPE_LOAD)
			mcpy8((void*)ph->vaddr, example + ph->offset, ph->mem_size);
	}

	dbg_printf("\nJumping to program entry point 0x%hd\n", fh->entry);
	dbg_printf("Returned %id\n", ((int(*)())fh->entry)());


	dbg_puts("\nFinal kernel memory map:\n");
	pmman_print_map(&pmman_kernel_map);

	dbg_puts("\n--- DordOS started successfully! ---\n");

	while (1)
		hlt();
}
