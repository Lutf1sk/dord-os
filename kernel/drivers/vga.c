#include <memory.h>
#include <debug_io.h>

#include <drivers/vga.h>

framebuf_t vga_framebuf;

void vga_initialize(void* vram, u32 res_x, u32 res_y) {
	dbg_printf("\nInitializing VGA...\n");
	dbg_printf(DBG_GRY"VRAM Address: 0x%hz\n"DBG_RST, vram);
	dbg_printf(DBG_GRY"Resolution: %idx%id\n"DBG_RST, res_x, res_y);
	vga_framebuf.px_data = vram;
	vga_framebuf.px_count = res_x * res_y;
	vga_framebuf.width = res_x;
	vga_framebuf.height = res_y;
}

