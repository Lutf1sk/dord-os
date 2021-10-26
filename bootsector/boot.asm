STAGE2_ADDR EQU (0x7F00)

MIB_ADDR EQU (0x8000)

extern read_sectors
extern panic
extern boot_drive

SECTION .boot
global boot
boot:
	jmp init

SECTION .text
[BITS 16]
init:
	mov sp, 0xFFFF

	; Zero segments
	xor ax, ax
	mov ds, ax
	mov ss, ax
	mov es, ax
	mov gs, ax
	mov fs, ax

	; Set video mode
	mov ax, 0x4F02
	mov bx, 0x03
	int 0x10

	cmp al, 0x4F
	jne panic

	mov [boot_drive], dl

	; Print 'R'
	mov ah, 0x09
	mov al, 'R'
	mov bh, 0
	mov bl, 0x0F
	mov cx, 1
	int 0x10

	; Read
	mov di, STAGE2_ADDR
	mov si, 1
	mov cx, 1
	call read_sectors

	; Get stage 2 size in sectors
	mov ax, [STAGE2_ADDR]

	; Read stage 2 data
	mov di, STAGE2_ADDR
	mov si, 2
	mov cx, ax
	call read_sectors

	mov dl, [boot_drive]

	; Jump to stage 2
	jmp STAGE2_ADDR
