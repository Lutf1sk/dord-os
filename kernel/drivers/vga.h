#ifndef VGA_H
#define VGA_H

#include <common.h>

extern u32* vga_vram;
extern i32 vga_res_x;
extern i32 vga_res_y;
extern u32 vga_pixel_count;

void vga_initialize(void* vram, u32 res_x, u32 res_y);

void vga_clear(u32 color);

void vga_put_image(const void* data, i32 pos_x, i32 pos_y, i32 width, i32 height);
void vga_blend_image(const void* data, i32 x, i32 y, i32 w, i32 h);

void vga_draw_rect(i32 x, i32 y, i32 w, i32 h, u32 clr);

#endif
