
struc ModeInfoBlock
	; Mandatory
	.mode_attribs:	 resw 1
	.win_a_attribs:	 resb 1
	.win_b_attribs:	 resb 1
	.win_granularity:resw 1
	.win_size:		 resw 1
	.win_a_seg:		 resw 1
	.win_b_seg:		 resw 1
	.win_func_ptr:	 resd 1
	.scan_line_bytes:resw 1

	; VBE 1.2+
	.x_resolution:	 resw 1
	.y_resolution:	 resw 1
	.x_char_size:	 resb 1
	.y_char_size:	 resb 1
	.plane_count:	 resb 1
	.bits_per_px:	 resb 1
	.bank_count:	 resb 1
	.mem_model:		 resb 1
	.bank_size:		 resb 1
	.img_page_count: resb 1
	.reserved0:		 resb 1

	; Direct Color
	.r_mask_size:	resb 1
	.r_field_pos:	resb 1
	.g_mask_size:	resb 1
	.g_field_pos:	resb 1
	.b_mask_size:	resb 1
	.b_field_pos:	resb 1
	.resrv_mask_size:	resb 1
	.resrv_field_pos:	resb 1
	.dc_mode_info:		resb 1

	; VBE 2.0+
	.phys_base_addr resd 1
	.reserved1		resd 1
	.reserved2		resw 1

	; VBE 3.0+
endstruc
