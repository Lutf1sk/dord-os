[BITS 32]

SECTION .text

extern irq_handler

%macro def_irq_handler 1
global irq%1_handler_asm
irq%1_handler_asm:
	pusha
	push dword %1
	jmp irq_handler_end
%endmacro

irq_handler_end:
	call irq_handler
	add esp, 4
	popa
	iret

%assign i 0
%rep 48
	def_irq_handler i
	%assign i i + 1
%endrep
