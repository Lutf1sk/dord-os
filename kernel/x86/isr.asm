[BITS 32]

SECTION .text

extern interrupt_handler

%macro def_isr 1
global isr%1
isr%1:
	pusha
	mov ebp, esp

	; SYSV requires DF to be clear on function entry
	cld

	; Dynamically align the stack because x86 hates consistency
	and esp, ~0xF

	push dword %1
	jmp isr_end
%endmacro

isr_end:
	call interrupt_handler

	mov esp, ebp
	popa
	iretd

%assign i 0
%rep 48
	def_isr i
	%assign i i + 1
%endrep

extern sys_handler
global isr_sys
isr_sys:
	push edi
	push esi
	push ebx
	push edx
	push ecx
	push eax

	push esp
	call sys_handler;
	add esp, 8

	pop ecx
	pop edx
	pop ebx
	pop esi
	pop edi
	iretd

