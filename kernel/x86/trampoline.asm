[BITS 16]
[ORG 0x8000]

enter:
	cli
	cld

	xor eax, eax
	xor ebx, ebx
	mov ds, ax

	mov ax, [0x8508]
	mov [gdtr], ax
	mov eax, [0x8500]
	mov [gdtr + 2], eax

	lgdt [gdtr]

	mov eax, cr0
	or eax, 1
	mov cr0, eax

	jmp 8:prot

prot:
	[BITS 32]
	mov ax, 0x10
	mov ds, ax
	mov es, ax
	mov fs, ax
	mov gs, ax
	mov ss, ax

	mov dword [0xFD000004], 0xFFFFFFFF

	mov esp, 0x8500

	jmp [0x8510]

gdtr:
	dw 0
	dd 0
