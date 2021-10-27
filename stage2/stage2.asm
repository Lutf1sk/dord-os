[BITS 16]

extern build_memory_map
extern enable_a20
extern read_sectors
extern memmap
extern panic

extern stage2_c

%include "mib.asm"
%include "vbeib.asm"

SECTION .entry
global entry
entry:
	jmp stage2

SECTION .text
stage2:
	mov [boot_drive], dl

	; Initialize COM1
	mov dx, 0x3f8 + 1
	mov al, 0
	out dx, al

	mov dx, 0x3f8 + 3
	mov al, 0x80
	out dx, al

	mov dx, 0x3f8 + 0
	mov al, 0x03
	out dx, al

	mov dx, 0x3f8 + 1
	mov al, 0
	out dx, al

	mov dx, 0x3f8 + 3
	mov al, 0x03
	out dx, al

	mov dx, 0x3f8 + 2
	mov al, 0xC7
	out dx, al

	mov dx, 0x3f8 + 4
	mov al, 0x0B
	out dx, al
	mov al, 0x1E
	out dx, al

	mov dx, 0x3f8 + 0
	mov al, 0xAE
	out dx, al

	mov dx, 0x3f8 + 4
	mov al, 0x0F
	out dx, al

	; Get VBE info block
	mov ax, 0x4F00
	mov di, vbeib
	int 0x10
	cmp ax, 0x004F
	jne panic

	mov si, [vbeib + VBEInfoBlock.mode_ptr]

.loop:
	mov cx, [si]
	cmp word cx, 0xFFFF
	je panic

	; Get mode info block
	mov ax, 0x4F01
	mov di, mib
	int 0x10
	cmp ax, 0x004F
	jne panic

	add si, 2

	cmp byte [mib + ModeInfoBlock.bits_per_px], 32
	jne .loop
	cmp word [mib + ModeInfoBlock.x_resolution], 800
	jl .loop
	cmp word [mib + ModeInfoBlock.y_resolution], 600
	jl .loop

	sub si, 2
.endloop:

	mov ax, 0x4F02
	mov bx, [si]
	or bx, (1 << 14) ; Set LFB bit
	int 0x10
	cmp ax, 0x004F
	jne panic

	; Build memory map
	call build_memory_map

	; Enable A20
	call enable_a20

	; Load GDT
	cli
	lgdt [gdt_desc]

	; Set protected mode bit in cr0
	mov eax, cr0
	or eax, 1
	mov cr0, eax

	jmp CODE_SEG:protected_mode

%include "gdt.asm"

[BITS 32]
SECTION .text
protected_mode:
	jmp CODE_SEG:.reload_segments
.reload_segments:
	mov ax, 0x10
	mov ds, ax
	mov es, ax
	mov fs, ax
	mov gs, ax
	mov ss, ax

	mov byte [0xB8000], 'P'
	mov byte [0xB8001], 0xF

	; Enable SSE3
	mov eax, cr0
	and ax, 0xFFFB		; Clear CR0.EM
	or ax, 0x2			; Set CR0.MP
	mov cr0, eax
	mov eax, cr4
	or ax, 3 << 9		; Set CR4.OSFXSR and CR4.OSXMMEXCPT
	mov cr4, eax

	mov esp, stack.top

	call stage2_c

	; Jump to the kernel entry point returned by stage2_c
	mov esi, boot_info
	jmp eax

SECTION .data
kernel_path: db "kernel.bin", 0
kernel_header_addr: dd 0

global klow
global khigh
global memmap_entries;

boot_info:
	klow:				dd 0
	khigh:				dd 0
	memmap_addr:		dd memmap
	mib_addr:			dd mib
	memmap_entries:		dw 0
	boot_drive:			db 0

SECTION .bss
stack: resb 1024
	.top:

mib: resb 256
vbeib: resb 256

