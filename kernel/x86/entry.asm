extern kernel_enter

[BITS 32]
SECTION .entry
global start
start:
	mov [boot_info_ptr], esi
	mov esp, stack.top

	call kernel_enter

hang:
	hlt
	jmp hang

SECTION .bss align=16
stack:
	resb 0x1000
.top:
global boot_info_ptr
boot_info_ptr: resb 8


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

