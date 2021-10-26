
SECTION .data

gdt:
	dq 0x0
gdt_code:
	dw 0xFFFF
	dw 0x0
	db 0x0
	db 10011010b
	db 11001111b
	db 0x0
gdt_data:
	dw 0xFFFF
	dw 0x0
	db 0x0
	db 10010010b
	db 11001111b
	db 0x0
gdt_end:

gdt_desc:
	dw gdt_end - gdt
	dd gdt

CODE_SEG equ gdt_code - gdt
DATA_SEG equ gdt_data - gdt

