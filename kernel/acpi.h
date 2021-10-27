#ifndef ACPI_H
#define ACPI_H 1

#include "common.h"

// Generic 'System Descriptor Table' header
typedef
struct acpi_sdt_header {
	char signature[4];
	u32 len;
	u8 rev;
	u8 checksum;
	char oem[6];
	char oem_table_id[8];
	u32 oem_rev;
	u32 creator_id;
	u32 creator_rev;
} acpi_sdt_header_t;

// Root System Descriptor Pointer
typedef
struct acpi_rdsp {
	char signature[8];
	u8 checksum;
	char oem[6];
	u8 rev;
	u32 rsdt_addr;
} acpi_rsdp_t;

// Version 2
typedef
struct acpi_rsdp2 {
	acpi_rsdp_t r1; // First (Rev. 1) part
	u32 len;
	u64 xsdt_addr;
	u8 ext_checksum;
	u8 reserved[3];
} acpi_rsdp2_t;


// Root System Descriptor Table
typedef
struct acpi_rsdt {
	acpi_sdt_header_t header;
	u32 sdt_addr_table[];
} acpi_rsdt_t;


// Multiple APIC Description Table
#define ACPI_MADT_CPU_LAPIC			0x00
#define ACPI_MADT_IO_APIC			0x01
#define ACPI_MADT_INTR_SRC_OVERR	0x02
#define ACPI_MADT_NMI_SRC			0x03
#define ACPI_MADT_LAPIC_NMI			0x04
#define ACPI_MADT_LAPIC_ADDR_OVERR	0x05
#define ACPI_MADT_LAPIC_X2			0x09

typedef
struct acpi_madt {
	acpi_sdt_header_t header;
	u32 lapic_addr;
	u32 flags;
	u8 intr_devs[];
} acpi_madt_t;


void acpi_initialize(void);

#endif
