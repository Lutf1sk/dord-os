OUTPUT_FORMAT("binary")

/* The bootloader will look at this image and start execution at the symbol
   designated as the entry point. */
ENTRY(entry)

/* Tell where the various sections of the object files will be put in the final
   kernel image. */
SECTIONS
{
	. = 0x7F00;

	.text :
	{
			*(.entry)
		*(.text)
	}

	/* Read-only data. */
	.rodata :
	{
		*(.rodata)
	}

	/* Read-write data (initialized) */
	.data :
	{
		*(.data)
	}

	/* Read-write data (uninitialized) and stack */
	.bss :
	{
		*(COMMON)
		*(.bss)
	}

	/* The compiler may produce other sections, by default it will put them in
	   a segment with the same name. Simply add stuff here as needed. */
}
