#ifndef PSF_H
#define PSF_H

#include "common.h"

#define PSF1_MAGIC 0x0436
#define PSF2_MAGIC 0x864ab572

#define PSF1_MODE512    0x01
#define PSF1_MODEHASTAB 0x02
#define PSF1_MODEHASSEQ 0x04
#define PSF1_MAXMODE    0x05

#define PSF1_SEPARATOR  0xFFFF
#define PSF1_STARTSEQ   0xFFFE

typedef
struct psf1_header {
	u16 magic;
	u8 mode;
	u8 height;
} psf1_header_t;

#define PSF2_UNICODE_TAB 0x01

#define PSF2_SEPARATOR  0xFF
#define PSF2_STARTSEQ   0xFE

typedef
struct psf2_header {
	u32 magic;
	u32 version;
	u32 header_size;
	u32 flags;
	u32 glyph_count;
	u32 glyph_bytes;
	u32 height;
	u32 width;
} psf2_header_t;

typedef
struct font {
	u16 width, height;
	u32 glyph_count;
	u32* glyph_data;
} font_t;

font_t* psf_load(void* out, void* data, usz len);

#endif
