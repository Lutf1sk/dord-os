#ifndef RENDER_H
#define RENDER_H 1

#include <common.h>

typedef
struct framebuf {
	i32 width, height;
	u32* px_data;
	usz px_count;
} framebuf_t;

void render_clear(framebuf_t* fb, u32 color);

void render_put_image(framebuf_t* fb, const void* data, i32 pos_x, i32 pos_y, i32 width, i32 height);
void render_blend_image(framebuf_t* fb, const void* data, i32 x, i32 y, i32 w, i32 h);

void render_rect(framebuf_t* fb, i32 x, i32 y, i32 w, i32 h, u32 clr);

#endif
