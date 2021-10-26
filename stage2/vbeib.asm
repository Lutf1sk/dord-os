
struc VBEInfoBlock
	.signature: resb 4
	.version: resw 1
	.oem_str: resw 2
	.capabilities: resb 4
	.mode_ptr: resw 2
	.total_vmem: resw 1
endstruc
