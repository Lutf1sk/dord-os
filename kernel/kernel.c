#include <memory.h>
#include <debug_io.h>
#include <asm.h>
#include <pmman.h>
#include <elf.h>
#include <proc.h>
#include <syscall.h>
#include <spinlock.h>

#include <x86/interrupts.h>
#include <x86/cpuid.h>
#include <x86/idt.h>
#include <x86/gdt.h>
#include <x86/msr.h>

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
#include <drivers/apic.h>

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

void kernel_proc(void);

proc_t* kproc;

slock_t ap_init_lock;

void ap_enter(void) {
	lidt(&idt_desc);
	sti();

	dbg_printf(DBG_GRY"CPU%ud: Success!\n"DBG_RST, cpu_lapic_id());
	spinlock_release(&ap_init_lock);

	while (1)
		hlt();
}

void kernel_enter(void) {
	dbg_puts(DBG_CYN"\nENTERED KERNEL\n"DBG_RST);
	dbg_printf("CPU: '%s' %s\n", cpu_vendor_str(), cpu_brand_str());

	cpu_info_t cpuinf;
	cpu_info(&cpuinf);

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
	idt_desc.addr = (usz)idt;
	lidt(&idt_desc);

	// Initialize ACPI
	acpi_initialize();

	// Remap PIC
	pic_initialize();

	// Find APIC base address
	void* lapic_base = NULL, *ioapic_base = (void*)APIC_IO_BASE_DEFAULT;
	if (acpi_lapic) {
		lapic_base = acpi_lapic;
		ioapic_base = acpi_ioapic;
	}
	else if (cpuinf.msr && cpuinf.apic) {
		u32 eax, edx;
		rdmsr(MSR_APIC_BASE, &eax, &edx);
		edx |= 1 << 11;
		wrmsr(MSR_APIC_BASE, eax, edx);
		lapic_base = (void*)(eax & 0xfffff000);
	}

 	// Initialize APIC if it is available
	if (lapic_base) {
		dbg_printf(DBG_GRY"Interrupt mode: APIC\n"DBG_RST);
		interrupt_mode = INTM_APIC;
		pic_mask_all();

		apic_initialize(lapic_base, ioapic_base);
	}
	else { // Otherwise, unmask all PIC IRQs
		dbg_printf(DBG_GRY"Interrupt mode: Legacy PIC\n"DBG_RST);
		pic_unmask_all();
	}

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

	// Start APs
	typedef
	struct ap_trampoline_data {
		u64 gdt_addr;
		u64 gdt_len;
		u64 entry_point;
	} ap_trampoline_data_t;

	#define AP_TRAMPOLINE_CODE ((void*)0x8000)
	#define AP_TRAMPOLINE_DATA ((void*)0x8500)

	extern void _binary_x86_trampoline_bin_start(void);
	extern void _binary_x86_trampoline_bin_end(void);
	extern void _binary_x86_trampoline_bin_size(void);

	mcpy32(AP_TRAMPOLINE_CODE, _binary_x86_trampoline_bin_start, (usz)_binary_x86_trampoline_bin_size);

	ap_trampoline_data_t* td = AP_TRAMPOLINE_DATA;
	td->entry_point = (usz)ap_enter;
	td->gdt_len = sizeof(gdt);
	td->gdt_addr = (usz)gdt;

	dbg_printf("\nInitializing application cores...\n");

	usz bsp_id = cpu_lapic_id();

	for (usz i = 0; i < acpi_cpu_count; ++i) {
		if (acpi_lapic_ids[i] != bsp_id) {
			spinlock_lock(&ap_init_lock);
			apic_start_core(acpi_lapic_ids[i]);
		}
	}
	spinlock_lock(&ap_init_lock);

	// Create and switch to kernel process
	kproc = proc_create(kernel_proc);
	cli();
	proc_switch(kproc);

	panic("Failed to enter kernel process");
}

#include <syscall.h>

void proc_dummy(void) {
	while (1) {
// 		dbg_printf("1\n");
	}
}

void proc_dummy2(void) {
	while (1) {
// 		dbg_printf("2\n");
	}
}

void kernel_proc(void) {
	pmman_map_t* pmkmap = &pmman_kernel_map;

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

	proc_t* child = proc_create(proc_dummy2);
	proc_register(child);

	child = proc_create(proc_dummy);
	proc_register(child);

	proc_exit();

	panic("Reached end of kernel\n");
}

