#include <memory.h>
#include <debug_io.h>

#include <drivers/vga.h>

u32* vga_vram = 0;
i32 vga_res_x = 0;
i32 vga_res_y = 0;
u32 vga_pixel_count = 0;

void vga_initialize(void* vram, u32 res_x, u32 res_y) {
	dbg_printf("\nInitializing VGA...\n");
	dbg_printf(DBG_GRY"VRAM Address: 0x%hz\n"DBG_RST, vram);
	dbg_printf(DBG_GRY"Resolution: %idx%id\n"DBG_RST, res_x, res_y);
	vga_vram = vram;
	vga_res_x = res_x;
	vga_res_y = res_y;
	vga_pixel_count = res_x * res_y;
}

void vga_clear(u32 color) {
	mset32(vga_vram, color, vga_pixel_count);
}

#define CHECK_IMAGE_BOUNDS() \
	if (x > vga_res_x || x + w < 0) \
		return; \
	if (y > vga_res_y || y + h < 0) \
		return; \
 \
	i32 uv_x = 0; \
	if (x < 0) { \
		uv_x = -x; \
		x = 0; \
	} \
 \
	i32 uv_w = (w - uv_x); \
	if (x + w > vga_res_x) \
		uv_w += vga_res_x - (x + w); \
 \
	i32 uv_y = 0; \
	if (y < 0) { \
		uv_y = -y; \
		y = 0; \
	} \
 \
	i32 uv_h = (h - uv_y); \
	if (y + h > vga_res_y) \
		uv_h += vga_res_y - (y + h); \
 \
	if (uv_h <= 0 || uv_w <= 0) \
		return;

void vga_put_image(const void* data, i32 x, i32 y, i32 w, i32 h) {
	CHECK_IMAGE_BOUNDS();

	// Write image data
	u32* vram_it = &vga_vram[x + (vga_res_x * y)];
	const u32* img_it = (u32*)data + (uv_y * w + uv_x);
	for (u32 i = 0; i < uv_h; ++i) {
		mcpy32(vram_it, img_it, uv_w);
		img_it += w;
		vram_it += vga_res_x;
	}
}

void vga_blend_image(const void* data, i32 x, i32 y, i32 w, i32 h) {
	CHECK_IMAGE_BOUNDS();

	// Write image data
	u32* vram_it = &vga_vram[x + (vga_res_x * y)];
	const u32* img_it = (u32*)data + (uv_y * w + uv_x);
	for (u32 i = 0; i < uv_h; ++i) {
		for (u32 i = 0; i < uv_w; ++i) {
			u32 clr = img_it[i];
			u8 a = clr >> 24;
			u32 blnd = (clr << ~a) + (vram_it[i] << a);

			vram_it[i] = blnd;
		}
		img_it += w;
		vram_it += vga_res_x;
	}
}

void vga_draw_rect(i32 x, i32 y, i32 w, i32 h, u32 clr) {
	CHECK_IMAGE_BOUNDS();

	u32* vram_it = &vga_vram[x + (vga_res_x * y)];

	for (u32 i = 0; i < uv_h; ++i) {
		mset32(vram_it, clr, uv_w);
		vram_it += vga_res_x;
	}
}

