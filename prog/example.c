#include <elf.h>

u8 streq(char* s1, char* s2) {
	for (;;) {
		if (*s1 != *s2)
			return 0;
		if (!*s1)
			return 1;
		++s1, ++s2;
	}
}

typedef usz (*dbg_printf_t)(char*,...);
dbg_printf_t dbg_printf = null;

int start(void) {
	u8* elf = 0x200000;
	elf32_fh_t* fh = 0x200000;
	elf32_sh_t* shs = (elf32_sh_t*)(elf + fh->sh_offset);

	elf32_sym_t* symtab = null;
	u8* strtab = null;

	for (usz i = 0; i < fh->sh_count; ++i) {
		if (shs[i].type == ELF_SH_TYPE_SYMTAB)
			symtab = (elf32_sym_t*)(elf + shs[i].offset);
		else if (shs[i].type == ELF_SH_TYPE_STRTAB) {
			strtab = (elf + shs[i].offset);
			break;
		}
	}

	for (usz i = 0;; ++i) {
		if (streq(strtab + symtab[i].name_stab_offs, "dbg_printf")) {
			dbg_printf = (dbg_printf_t)symtab[i].value;
			break;
		}
	}

	dbg_printf("Skjut mig\n");
	return -69;
}

