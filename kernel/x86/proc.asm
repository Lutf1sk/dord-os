
extern proc_current
extern proc_end
extern proc_release

extern pit_systime_msec

struc proc
	.sp:		resd 1
	.entry:		resd 1
	.next:		resd 1
	.time_end:	resd 1
	.name:		resd 1
endstruc

SECTION .text
global proc_switch
proc_switch: ;	void proc_switch(proc_t* proc)
	; Push current state
	push ebx
	push esi
	push edi
	push ebp
	pushfd

	; Save current stack pointer
	mov edi, [proc_current]
	test edi, edi
	jz .no_current
	mov [edi + proc.sp], esp
.no_current:

	mov esi, [esp + (6 * 4)] ; Read parameter from stack

	; Set time slice
	mov edi, [pit_systime_msec]
	add edi, 8
	mov [esi + proc.time_end], edi

	; Load new stack pointer
	mov esp, [esi + proc.sp]

	; Set the new process as current
	mov [proc_current], esi

	; Pop new state
	popfd
	pop ebp
    pop edi
    pop esi
    pop ebx
	ret

proc_startup:
	push dword 0x200 | 0x200000
	call proc_release
	add esp, 4
	ret

global proc_init
proc_init:
	push ebp
	mov ebp, esp
	mov edi, [ebp + 8] ; Read parameter from stack

	mov esp, [edi + proc.sp]	; Set stack pointer

	; Prepare pop-ed data
	push dword 0
	push dword [edi + proc.entry]
	push dword proc_startup
	push dword 0 ; bx
	push dword 0 ; si
	push dword 0 ; di
	push dword 0 ; bp
	push dword 0 ; eflags

	mov [edi + proc.sp], esp	; Restore stack pointer

	mov esp, ebp
	pop ebp
	ret

