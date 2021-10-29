OUTPUT_FORMAT("elf32-i386")

/* The bootloader will look at this image and start execution at the symbol
   designated as the entry point. */
ENTRY(start)

/* Tell where the various sections of the object files will be put in the final
   kernel image. */
SECTIONS
{
	. = 0x180000;

	.text BLOCK(4K) : ALIGN(4K)
	{
		*(.text)
	}

	.data BLOCK(4K) : ALIGN(4K)
	{
		*(.data)
	}

	.rodata BLOCK(4K) : ALIGN(4K)
	{
		*(.rodata)
	}

	.bss BLOCK(4K) : ALIGN(4K)
	{
		*(.bss)
		*(COMMON)
	}
}
