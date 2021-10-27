#include "debug_io.h"
#include "pci.h"
#include "ide.h"
#include "asm.h"
#include "memory.h"
#include "debug_io.h"

#include <elf.h>

NORETURN
void panic_c(const char* str) {
	dbg_printf(DBG_RED"STAGE 2 PANIC: %s\n"DBG_RST, str);

	for (;;) {
		cli();
		hlt();
	}
}

ide_drive_t* drives;
u32 drive_count;

void enumerate_and_initialize(void) {
	u32 regs[PCI_HEADER_MAX_SIZE];

	for (u16 bus = 0; bus < PCI_MAX_BUS; bus++) {
		for (u8 device = 0; device < PCI_MAX_DEVICE; device++) {
			for (u8 func = 0; func < PCI_MAX_FUNC; func++) {

				u16 res = pci_read_reg(bus, device, func, 0x00);

				if (res != 0xFFFF) {
					u32* it = regs;
					for (u32 i = 0; i < 20; i++)
						*it++ = pci_read_reg(bus, device, func, i << 2);

					// If it is an IDE controller, initialize it and then return
					if (pci_reg8(regs, PCI_HC_HEADER_TYPE) == PCI_HEADER_TYPE_0 &&
						pci_reg8(regs, PCI_HC_CLASS) == 0x01 &&
						pci_reg8(regs, PCI_HC_SUBCLASS) == 0x01)
					{
						drives = ide_initialize(
							&drive_count,
							pci_reg32(regs, PCI_H0_BAR0),
							pci_reg32(regs, PCI_H0_BAR1),
							pci_reg32(regs, PCI_H0_BAR2),
							pci_reg32(regs, PCI_H0_BAR3),
							pci_reg32(regs, PCI_H0_BAR4)
						);
						return;
					}
				}
			}
		}
	}

	panic_c("No usable IDE Controller found");
}

void* load_elf_kernel(u8* kernel_elf_data) {
	elf32_fh_t* fh = (elf32_fh_t*)kernel_elf_data;

	char magic[4] = { 0x7F, 'E', 'L', 'F' };
	if (!strneq(fh->magic, magic, 4))
		panic_c("Kernel has invalid elf signature");

	elf32_sh_t* sh = (elf32_sh_t*)&kernel_elf_data[fh->sh_offset];

	char* strtab = (char*)&kernel_elf_data[sh[fh->sh_strtab_index].offset];

	for (u32 i = 0; i < fh->sh_count; ++i) {
		void* addr = (void*)sh[i].addr;
		u32 size = sh[i].size;
		void* data = &kernel_elf_data[sh[i].offset];
		char* name = &strtab[sh[i].name_stab_offs];

		if (sh[i].type == ELF_SH_TYPE_PROGBITS && addr) {
			dbg_printf(DBG_GRY"Loading section '%s'\n"DBG_RST, name);
			mcpy8(addr, data, size);
		}
		else if (sh[i].type == ELF_SH_TYPE_NOBITS) {
			dbg_printf(DBG_GRY"Zeroing section '%s'\n"DBG_RST, name);
			mset8(addr, 0, size);
		}
	}

	dbg_printf("Jumping to kernel entry point 0x%hz\n", fh->entry);
	return (void*)fh->entry;
}

#define TMP_ELF_ADDR ((void*)0x200000)

typedef
struct PACKED dfs_file_header {
	u64 sectors, next_lba;
	char name[32];
} dfs_file_header_t;

void* stage2_c(void) {
	dbg_printf(DBG_CYN"\nENTERED STAGE 2\n"DBG_RST);
	enumerate_and_initialize();

	for (i32 i = 0; i < drive_count; ++i) {
		dbg_printf("Checking drive %ud:\n", i);

		char buf[512];
		dfs_file_header_t* header = (dfs_file_header_t*)buf;
		u64 lba = 1;

		while (lba) {
			ide_read_drive(&drives[i], buf, lba, 1);

			if (strneq(header->name, "kernel.bin", 32)) {
				dbg_printf(DBG_GRN"Found kernel image '%s' on drive %ud\n"DBG_RST, header->name, i);

				ide_read_drive(&drives[i], TMP_ELF_ADDR, lba + 1, header->sectors);
				return load_elf_kernel(TMP_ELF_ADDR);
			}

			lba = header->next_lba;
		}
	}

	panic_c("No kernel image found\n");
}
