[BITS 16]

SECTION .text
global panic
panic:
	; Print red 'E'
	mov ah, 0x09
	mov al, 'E'
	mov bh, 0
	mov bl, 0x0C
	mov cx, 1
	int 0x10

	; Hang forever
.hang:
	cli
	hlt
	jmp .hang
