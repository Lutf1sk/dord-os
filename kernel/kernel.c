#include <memory.h>
#include <debug_io.h>
#include <asm.h>
#include <pmman.h>
#include <elf.h>
#include <proc.h>
#include <syscall.h>
#include <spinlock.h>
#include <psf.h>
#include <ppm.h>

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

u32 kproc_flags = 0;

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

void proc_wserver(void);
void proc_wclient(void);

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

		apic_initialize(lapic_base, ioapic_base, acpi_intr_mappings);
	}
	else { // Otherwise, unmask all PIC IRQs
		dbg_printf(DBG_GRY"Interrupt mode: Legacy PIC\n"DBG_RST);
		pic_unmask_all();
	}

	sti(); // Enable interrupts

	// Initialize VGA framebuffer
	vga_initialize((void*)boot_info.mib->phys_base_addr, boot_info.mib->x_resolution, boot_info.mib->y_resolution);
	render_clear(&vga_framebuf, 0x000000);

	mouse_x = vga_framebuf.width / 2;
	mouse_y = vga_framebuf.height / 2;

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
			drives = ide_initialize(
				pci_reg32(dev, PCI_H0_BAR0), pci_reg32(dev, PCI_H0_BAR1),
				pci_reg32(dev, PCI_H0_BAR2), pci_reg32(dev, PCI_H0_BAR3),
				pci_reg32(dev, PCI_H0_BAR4),
				&drive_count
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
	if (lapic_base && acpi_cpu_count > 0) {
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
	}

	cli();

	// Create root filesystem
	dfs_t dfs_root;
	dfs_init(&dfs_root, &drives[0].interf);
	rootfs = (fs_t*)&dfs_root;

	proc_init_scheduler();

	proc_t* child = proc_create(proc_wserver, "wserver");
	proc_register(child);

	child = proc_create(proc_wclient, "wclient");
	proc_register(child);

	proc_schedule();
}

// Window server

#define B 0xFFFFFFFF,
#define W 0xFF000000,
#define _ 0x00000000,

u32 cursor[24][13] = {
	{ B _ _ _ _ _ _ _ _ _ _ _ _ },
	{ B B _ _ _ _ _ _ _ _ _ _ _ },
	{ B W B _ _ _ _ _ _ _ _ _ _ },
	{ B W W B _ _ _ _ _ _ _ _ _ },
	{ B W W W B _ _ _ _ _ _ _ _ },
	{ B W W W W B _ _ _ _ _ _ _ },
	{ B W W W W W B _ _ _ _ _ _ },
	{ B W W W W W W B _ _ _ _ _ },
	{ B W W W W W W W B _ _ _ _ },
	{ B W W W W W W W W B _ _ _ },
	{ B W W W W W W W W W B _ _ },
	{ B W W W W W W W W W W B _ },
	{ B W W W W W W B B B B B B },
	{ B W W W W W W B _ _ _ _ _ },
	{ B W W B B W W W B _ _ _ _ },
	{ B W B _ B W W W B _ _ _ _ },
	{ B B _ _ _ B W W W B _ _ _ },
	{ B _ _ _ _ B W W W B _ _ _ },
	{ _ _ _ _ _ _ B B B _ _ _ _ },
	{ _ _ _ _ _ _ _ _ _ _ _ _ _ },
};

typedef
struct rect {
	i32 x, y;
	i32 w, h;
} rect_t;

#define RECT_INIT(x, y, w, h) { (x), (y), (w), (h) }
#define RECT(x, y, w, h) ((rect_t)RECT_INIT(x, y, w, h))

font_t font;

typedef
struct win {
	rect_t r;
	u32* px_data;
	char* title;
	u8 dragged;
} win_t;

win_t w[16];
usz win_count = 0;

void ws_load_font(char* path) {
	void* file_data = null;
	usz file_size = 0;
	if (fread(path, null, &file_size))
		panic("Failed to read font file size\n");
	file_data = pmman_alloc(&pmman_kernel_map, file_size);
	if (fread(path, file_data, &file_size))
		panic("Failed to read font file data\n");

	if (psf_load(file_data, file_size, &font, null))
		panic("Failed to load font\n");
	void* font_data = pmman_alloc(&pmman_kernel_map, font.glyph_count * font.px_per_glyph * sizeof(u32));
	if (psf_load(file_data, file_size, &font, font_data))
		panic("Failed to load font\n");
}

img_t ws_load_img(char* path) {
	void* file_data = null;
	usz file_size = 0;
	if (fread(path, null, &file_size))
		panic("Failed to read image file size\n");
	file_data = pmman_alloc(&pmman_kernel_map, file_size);
	if (fread(path, file_data, &file_size))
		panic("Failed to read image file data\n");

	img_t img;
	if (ppm_load(file_data, file_size, &img, null) != OK)
		panic("Failed to load image");
	void* pxmap = pmman_alloc(&pmman_kernel_map, img.width * img.height * sizeof(u32));
	if (ppm_load(file_data, file_size, &img, pxmap) != OK)
		panic("Failed to load image");

	return img;
}

void ws_draw_border(framebuf_t* fb, rect_t* r, u32 clr) {
	render_rect(fb, r->x, r->y, 1, r->h, clr);
	render_rect(fb, r->x + r->w - 1, r->y, 1, r->h, clr);
	render_rect(fb, r->x, r->y, r->w, 1, clr);
	render_rect(fb, r->x, r->y + r->h - 1, r->w, 1, clr);
}

void ws_draw_text(framebuf_t* fb, i32 x, i32 y, char* str) {
	u8* it = (u8*)str;
	while (*it) {
		render_blend_image(fb, &font.glyph_data[font.px_per_glyph * (*it++)], x, y, font.glyph_width, font.glyph_height);
		x += font.glyph_width;
	}
}

void ws_draw_rect(framebuf_t* fb, rect_t* r, u32 clr) {
	render_rect(fb, r->x, r->y, r->w, r->h, clr);
}

u8 rect_contains(rect_t* r, i32 x, i32 y) {
	return (x >= r->x) && (x < r->x + r->w) && (y >= r->y) && (y < r->y + r->h);
}

void proc_wserver(void) {
	pmman_map_t* pmkmap = &pmman_kernel_map;

	void* double_buf = pmman_alloc(pmkmap, vga_framebuf.px_count * sizeof(u32));
	if (!double_buf)
		panic("Failed to allocate double buffer\n");

	framebuf_t fb = vga_framebuf;
	fb.px_data = double_buf;

	ws_load_font("lat1-16.psf");

	i32 drag_x = 0, drag_y = 0;
	i32 drag_wx = 0, drag_wy = 0;

	usz focus = 0;

	while (1) {
		render_clear(&fb, 0);

		for (usz i = 0; i < win_count; ++i) {
			if (w[i].dragged) {
				w[i].r.x = drag_wx + (mouse_x - drag_x);
				w[i].r.y = drag_wy + (mouse_y - drag_y);
			}

			rect_t bounds = RECT(w[i].r.x - 1, w[i].r.y - 20, w[i].r.w + 2, 20 + w[i].r.h + 2);

			if (rect_contains(&bounds, mouse_x, mouse_y)) {
				if (mouse_button_states[MS_BTN_LEFT]) {
					win_t win = w[i];
					mmove8(&w[1], &w[0], i * sizeof(win_t));
					w[0] = win;

					focus = 0;
				}
				else
					focus = i;
				break;
			}
		}

		for (i32 i = win_count - 1; i >= 0; --i) {
			rect_t hr = RECT(w[i].r.x - 1, w[i].r.y - 20, w[i].r.w + 2, 20);
			rect_t br = RECT(w[i].r.x - 1, w[i].r.y - 1, w[i].r.w + 2, w[i].r.h + 2);

			u32 bclr = 0x404040;

			if (i == focus) {
				bclr = 0xAE3030;

				if (rect_contains(&hr, mouse_x, mouse_y) && mouse_button_states[MS_BTN_LEFT] && !w[0].dragged) {
					w[i].dragged = 1;
					drag_x = mouse_x;
					drag_y = mouse_y;
					drag_wx = w[i].r.x;
					drag_wy = w[i].r.y;
				}
				else if (!mouse_button_states[MS_BTN_LEFT])
					w[i].dragged = 0;
			}

			ws_draw_rect(&fb, &hr, bclr);
			ws_draw_text(&fb, w[i].r.x + 4, w[i].r.y - 18, w[i].title);

			ws_draw_border(&fb, &br, bclr);
			render_put_image(&fb, w[i].px_data, w[i].r.x, w[i].r.y, w[i].r.w, w[i].r.h);
		}

		render_blend_image(&fb, cursor, mouse_x, mouse_y, 13, 24);
		mcpy32(vga_framebuf.px_data, double_buf, vga_framebuf.px_count);

		proc_sleep_msec(16);

	}
}

void proc_wclient(void) {
	w[0].r = RECT(100, 100, 800, 600);
	w[0].px_data = pmman_alloc(&pmman_kernel_map, w[0].r.w * w[0].r.h * sizeof(u32));
	w[0].title = "A window title";
	w[0].dragged = 0;

	w[1].r = RECT(100, 100, 400, 300);
	w[1].px_data = pmman_alloc(&pmman_kernel_map, w[1].r.w * w[1].r.h * sizeof(u32));
	w[1].title = "Another window title";
	w[1].dragged = 0;

	framebuf_t fbs[2] = {
		{ w[0].r.w, w[0].r.h, w[0].px_data, w[0].r.w * w[0].r.h },
		{ w[1].r.w, w[1].r.h, w[1].px_data, w[1].r.w * w[1].r.h },
	};

	win_count = 2;

	img_t img = ws_load_img("dord.ppm");

	u8 shade = 0x00;
	while (1) {
		for (usz i = 0; i < win_count; ++i) {
			framebuf_t* fb = &fbs[i];
			render_clear(fb, 0x202020);
			render_put_image(fb, img.px_data, 0, 0, img.width, img.height);
			ws_draw_text(fb, img.width, img.height, "Skjut mig");
		}
		proc_sleep_msec(16);
	}
}

