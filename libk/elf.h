#ifndef ELF_H
#define ELF_H 1

#include <common.h>

#define ELF_VERSION_CURRENT 1

typedef
enum elf_class {
	ELFCLASS_32 = 1,
	ELFCLASS_64 = 2,
} elf_class_t;

typedef
enum elf_osabi {
	ELFOSABI_SYSV = 0,
} elf_osabi_t;

typedef
enum elf_encoding {
	ELFENC_LSB = 1,
	ELFENC_MSB = 2,
} elf_encoding_t;

typedef
enum elf_objtype {
	ELFTYPE_REL = 1,
	ELFTYPE_EXEC = 2,
	ELFTYPE_DYN = 3,
	ELFTYPE_CORE = 4,
} elf_objtype_t;

typedef
enum elf_arch {
	ELFARCH_I386 = 3,
	ELFARCH_AMD64 = 62,
} elf_arch_t;

typedef
struct PACKED elf64_fh {
	u8 magic[4];

	u8 class;
	u8 encoding;
	u8 header_version;
	u8 osabi;

	u8 pad[8];

	u16 obj_type;
	u16 arch;
	u32 version;

	u64 entry;
	u64 ph_offset;
	u64 sh_offset;

	u32 cpu_flags;

	u16 fh_size;

	u16 ph_size;
	u16 ph_count;
	u16 sh_size;
	u16 sh_count;

	u16 sh_strtab_index;
} elf64_fh_t;

typedef
struct PACKED elf32_fh {
	u8 magic[4];

	u8 class;
	u8 encoding;
	u8 header_version;
	u8 osabi;

	u8 pad[8];

	u16 obj_type;
	u16 arch;
	u32 version;

	u32 entry;
	u32 ph_offset;
	u32 sh_offset;

	u32 cpu_flags;

	u16 fh_size;

	u16 ph_size;
	u16 ph_count;
	u16 sh_size;
	u16 sh_count;

	u16 sh_strtab_index;
} elf32_fh_t;

typedef
enum elf_ph_type {
	ELF_PH_TYPE_NULL = 0,
	ELF_PH_TYPE_LOAD = 1,
	ELF_PH_TYPE_DYNAMIC = 2,
	ELF_PH_TYPE_INTERP = 3,
	ELF_PH_TYPE_NOTE = 4,
	ELF_PH_TYPE_SHLIB = 5,
	ELF_PH_TYPE_PHDR = 6,
	ELF_PH_TYPE_TLS = 7,
} elf_ph_type_t;

typedef
enum elf_ph_flags {
	ELF_PH_X = 1,
	ELF_PH_W = 2,
	ELF_PH_R = 4,
} elf_ph_flags_t;

typedef
struct PACKED elf64_ph {
	u32 type;
	u32 flags;
	u64 offset;

	u64 vaddr;
	u64 paddr;
	u64 file_size;
	u64 mem_size;
	u64 alignment;
} elf64_ph_t;

typedef
struct PACKED elf32_ph {
	u32 type;
	u32 flags;
	u32 offset;

	u32 vaddr;
	u32 paddr;
	u32 file_size;
	u32 mem_size;
	u32 alignment;
} elf32_ph_t;

typedef
enum elf_sh_type {
	ELF_SH_TYPE_NULL = 0,
	ELF_SH_TYPE_PROGBITS = 1,
	ELF_SH_TYPE_SYMTAB = 2,
	ELF_SH_TYPE_STRTAB = 3,
	ELF_SH_TYPE_RELA = 4,
	ELF_SH_TYPE_HASH = 5,
	ELF_SH_TYPE_DYNAMIC = 6,
	ELF_SH_TYPE_NOTE = 7,
	ELF_SH_TYPE_NOBITS = 8,
	ELF_SH_TYPE_REL = 9,
	ELF_SH_TYPE_SHLIB = 10,
	ELF_SH_TYPE_DYNSYM = 11,
	ELF_SH_TYPE_INIT_ARRAY = 14,
	ELF_SH_TYPE_FINI_ARRAY = 15,
	ELF_SH_TYPE_PREINIT_ARRAY = 16,
	ELF_SH_TYPE_GROUP = 17,
	ELF_SH_TYPE_SYMTAB_SHNDX = 18,
	ELF_SH_TYPE_NUM = 19,
} elf_sh_type_t;

typedef
struct PACKED elf64_sh {
	u32 name_stab_offs;

	u32 type;
	u64 flags;

	u64 addr;
	u64 offset;
	u64 size;

	u32 link;
	u32 info;

	u64 addr_align;
	u64 ent_size;
} elf64_sh_t;

typedef
struct PACKED elf32_sh {
	u32 name_stab_offs;

	u32 type;
	u32 flags;

	u32 addr;
	u32 offset;
	u32 size;

	u32 link;
	u32 info;

	u32 addr_align;
	u32 ent_size;
} elf32_sh_t;

#endif
