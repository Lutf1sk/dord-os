#include <memory.h>
#include <debug_io.h>
#include <asm.h>
#include <pmman.h>

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

NORETURN
void panic(const char* str) {
	dbg_printf(DBG_RED"KERNEL PANIC: %s\n", str);
	pit_sleep_msec(10);
	for (;;) {
		cli();
		hlt();
	}
}

idt_t idt[256] = {};
idt_desc_t idt_desc;

gdt_t gdt[3];
gdt_desc_t gdt_desc;

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

	u32* dord;
	usz dord_w, dord_h;

	for (usz i = 0; i < drive_count; ++i) {
		dbg_printf("\n'%s' [%s %s %s] (%ud sectors):\n", drives[i].name,
				drive_type[drives[i].type], drive_channel[drives[i].channel], drive_bus[drives[i].bus], drives[i].size);

		if (drives[i].type != IDE_ATA) // Only search ATA drives (for now)
			continue;

		dfs_it_t it = dfs_it_begin(&drives[i]);
		while (dfs_iterate(&it)) {
			dbg_printf(DBG_GRY"File: '%s' (%ud Bytes)\n"DBG_RST, it.name, it.sectors * 512);

			if (strneq(it.name, "dord.bmp", 32)) {
				static u8 dord_file[4096 + 512];
				dfs_read(dord_file, &it);

				if (dord_file[0] != 'B' || dord_file[1] != 'M')
					dbg_puts("Invalid bmp magic, expected 'BM'\n");

				u32 datapos = *(int*)&(dord_file[0x0A]);
				u32 size = *(int*)&(dord_file[0x22]);
				dord_w = *(int*)&(dord_file[0x12]);
				dord_h = *(int*)&(dord_file[0x16]);

				dord = pmman_alloc(pmkmap, size);

				u32* dord_it = dord;
				u32* file_it = (u32*)&dord_file[datapos] + (dord_h - 1) * dord_w;
				for (i32 i = dord_h - 1; i >= 0; --i) {
					mcpy8(dord_it, file_it, dord_w * sizeof(u32));
					dord_it += dord_w;
					file_it -= dord_w;
				}
			}
		}
	}

	dbg_puts("\n--- DordOS started successfully! ---\n");

	vbe_mib_t* mib = boot_info.mib;

	u32 last_time = pit_time_msec();

	// Create a second vram-sized area for double buffering
	usz vram_size = vga_pixel_count * sizeof(u32);
	void* vram = vga_vram;
	vga_vram = pmman_alloc(pmkmap, vram_size);

	pmman_print_map(&pmman_kernel_map);

	extern i32 mouse_x, mouse_y;
	while (1) {
		u32 time = pit_time_msec();
		// Draw an image at the mouse position
		if (time % 17 == 0 && last_time != time) {
			last_time = time;
			vga_clear(0x000000);
			vga_put_image(dord, mouse_x, mouse_y, 32, 32);
			mcpy32(vram, vga_vram, vram_size);
		}
		hlt();
	}
}
