#ifndef VGA_H
#define VGA_H

#include <render.h>

extern framebuf_t vga_framebuf;

void vga_initialize(void* vram, u32 res_x, u32 res_y);

#endif
