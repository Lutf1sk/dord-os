#include <render.h>
#include <memory.h>

void render_clear(framebuf_t* fb, u32 color) {
	mset32(fb->px_data, color, fb->px_count);
}

#define CHECK_IMAGE_BOUNDS() \
	if (x > fb->width || x + w < 0) \
		return; \
	if (y > fb->height || y + h < 0) \
		return; \
 \
	i32 uv_x = 0; \
	if (x < 0) { \
		uv_x = -x; \
		x = 0; \
	} \
 \
	i32 uv_w = (w - uv_x); \
	if (x + w > fb->width) \
		uv_w += fb->width - (x + w); \
 \
	i32 uv_y = 0; \
	if (y < 0) { \
		uv_y = -y; \
		y = 0; \
	} \
 \
	i32 uv_h = (h - uv_y); \
	if (y + h > fb->height) \
		uv_h += fb->height - (y + h); \
 \
	if (uv_h <= 0 || uv_w <= 0) \
		return;

void render_put_image(framebuf_t* fb, const void* data, i32 x, i32 y, i32 w, i32 h) {
	CHECK_IMAGE_BOUNDS();

	// Write image data
	u32* vram_it = &fb->px_data[x + (fb->width * y)];
	const u32* img_it = (u32*)data + (uv_y * w + uv_x);
	for (u32 i = 0; i < uv_h; ++i) {
		mcpy32(vram_it, img_it, uv_w);
		img_it += w;
		vram_it += fb->width;
	}
}

void render_blend_image(framebuf_t* fb, const void* data, i32 x, i32 y, i32 w, i32 h) {
	CHECK_IMAGE_BOUNDS();

	// Write image data
	u32* vram_it = &fb->px_data[x + (fb->width * y)];
	const u32* img_it = (u32*)data + (uv_y * w + uv_x);
	for (u32 i = 0; i < uv_h; ++i) {
		for (u32 i = 0; i < uv_w; ++i) {
			u32 clr = img_it[i];
			u8 a = clr >> 24;
			u32 blnd = (clr << ~a) + (vram_it[i] << a);

			vram_it[i] = blnd;
		}
		img_it += w;
		vram_it += fb->width;
	}
}

void render_rect(framebuf_t* fb, i32 x, i32 y, i32 w, i32 h, u32 clr) {
	CHECK_IMAGE_BOUNDS();

	u32* vram_it = &fb->px_data[x + (fb->width * y)];

	for (u32 i = 0; i < uv_h; ++i) {
		mset32(vram_it, clr, uv_w);
		vram_it += fb->width;
	}
}


