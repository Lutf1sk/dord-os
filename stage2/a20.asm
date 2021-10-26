[BITS 16]

extern panic

SECTION .text
global enable_a20
enable_a20:
	pushf
	push ds
	push es
	push di
	push si
	push ax

	call check_a20

	in al, 0x92
	or al, 2
	out 0x92, al

	call check_a20

	in al, 0xEE

	call check_a20

	; Panic if the A20 line could not be enabled
	jmp panic

success:
	pop ax ; Pop ax twice to skip past the ret ip
	pop ax
	pop si
	pop di
	pop es
	pop ds
	popf
	ret


check_a20:
	cli

	xor ax, ax ; ax = 0
	mov es, ax

	not ax ; ax = 0xFFFF
	mov ds, ax

	mov di, 0x0500
	mov si, 0x0510

	mov al, byte [es:di]
	push ax

	mov al, byte [ds:si]
	push ax

	mov byte [es:di], 0x00
	mov byte [ds:si], 0xFF

	cmp byte [es:di], 0xFF

	pop ax
	mov byte [ds:si], al

	pop ax
	mov byte [es:di], al

	jne success
	ret
