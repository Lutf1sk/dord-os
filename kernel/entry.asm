[BITS 32]

extern kernel_enter

SECTION .entry
global start
start:
	mov [boot_info.memmap_addr], ecx
	mov [boot_info.mib_addr], edi
	mov [boot_info.memmap_entries], bx
	mov [boot_info.boot_drive], dl
	mov esp, stack.top
	mov [boot_info.stack], esp

	call kernel_enter

hang:
	hlt
	jmp hang

SECTION .bss align=16
stack:
	resb 0x10000
.top:

SECTION .data
global boot_info
boot_info:
	.stack:				dd 0
	.memmap_addr:		dd 0
	.mib_addr:			dd 0
	.memmap_entries:	dw 0
	.boot_drive:		db 0

SECTION .text
global reload_segments_asm
reload_segments_asm:
	jmp 0x08:.reload_segments
.reload_segments:
	mov ax, 0x10
	mov ds, ax
	mov es, ax
	mov fs, ax
	mov gs, ax
	mov ss, ax
	ret

