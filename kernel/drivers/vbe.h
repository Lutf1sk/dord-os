#ifndef VBE_H
#define VBE_H

#include <common.h>

typedef
struct vbe_infb {
	char signature[4];
	u16 version;
	u32 oem;
	u8 capabilities[4];
	u32 mode_ptr;
	u16 total_vmem;
} vbe_infb_t;

typedef
struct vbe_mib {
	// Mandatory
	u16 mode_attribs;
	u8 win_a_attribs;
	u8 win_b_attribs;
	u16 win_granularity;
	u16 win_size;
	u16 win_a_seg;
	u16 win_b_seg;
	u32 win_func_ptr;
	u16 scan_line_bytes;

	// VBE 1.2+
	u16 x_resolution;
	u16 y_resolution;
	u8 x_char_size;
	u8 y_char_size;
	u8 plane_count;
	u8 bits_per_px;
	u8 bank_count;
	u8 mem_model;
	u8 bank_size;
	u8 img_page_count;
	u8 reserved0;

	// Direct Color
	u8 r_mask_size;
	u8 r_field_pos;
	u8 g_mask_size;
	u8 g_field_pos;
	u8 b_mask_size;
	u8 b_field_pos;
	u8 resrv_mask_size;
	u8 resrv_field_pos;
	u8 dc_mode_info;

	// VBE 2.0+
	u32 phys_base_addr;
	u32 reserved1;
	u16 reserved2;

	// VBE 3.0+
} vbe_mib_t;

#endif
