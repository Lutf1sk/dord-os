[BITS 16]

extern panic

SECTION .text
global read_sectors
read_sectors:
	push si ; LBA
	push di ; DST Address
	push cx ; Sector count

	push ax
	push dx

	mov [dap.sector_count], cx
	mov [dap.dst_addr], di
	mov [dap.start_lba], si

	mov ah, 0x42
	mov si, dap
	mov dl, [boot_drive]
	int 0x13

	jc panic

	pop dx
	pop ax

	pop cx
	pop di
	pop si
	ret

SECTION .data
dap:
	db 0x10
	db 0
.sector_count: dw 0
.dst_addr: dd 0
.start_lba: dq 0

global boot_drive
boot_drive:
	db 0
