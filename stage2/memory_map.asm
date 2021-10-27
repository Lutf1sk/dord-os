[BITS 16]

extern panic
extern memmap_entries

SECTION .text

global mset
mset:
	rep stosb
	ret

global build_memory_map
build_memory_map:
	push di
	push ebx
	push edx
	push ecx
	push eax
	push si

	mov al, 0
	mov cx, 256
	mov di, memmap
	call mset

	xor esi, esi
	mov si, -1

	mov di, memmap - 24
	xor ebx, ebx
	mov edx, 0x534D4150

.loop:
	inc si
	add di, 24

	mov eax, 0xE820
	mov ecx, 24
	int 0x15

	jc panic
	cmp eax, 0x534D4150
	jne panic
	cmp ebx, 0
	jne .loop

	mov [memmap_entries], si

	pop si
	pop eax
	pop ecx
	pop edx
	pop ebx
	pop di
	ret

SECTION .data
global memmap
memmap: times (32 * 3) dq 0

