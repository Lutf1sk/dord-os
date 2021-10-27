#ifndef MEMMAP_H
#define MEMMAP_H

#include <common.h>

typedef
enum memmap_type {
	MEMMAP_INVAL = 0,
	MEMMAP_USABLE = 1,
	MEMMAP_RESERVED = 2,
	MEMMAP_ACPI_RECLAIMABLE = 3,
	MEMMAP_ACPI_NVS = 4,
	MEMMAP_BAD_MEMORY = 5,
} memmap_type_t;

typedef
struct PACKED memmap {
	u64 base;
	u64 len;
	u32 type;
	u32 attribs;
} memmap_t;

static INLINE
const char* memmap_type_str(u32 type) {
	switch (type) {
	case MEMMAP_INVAL: return "INVALID";
	case MEMMAP_USABLE: return "USABLE";
	case MEMMAP_RESERVED: return "RESERVED";
	default: return "UNKNOWN";
	}
}

#endif
