#ifndef PPM_H
#define PPM_H 1

#include <err.h>

#define PPM_MAGIC 0x3650

typedef struct img {
	u32 width, height;
	usz px_count;
	u32* px_data;
} img_t;

err_t ppm_load(void* data, usz len, img_t* out_img, u32* out_data);

#endif
