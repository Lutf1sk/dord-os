#include "acpi.h"
#include "memory.h"
#include "debug_io.h"

static
u8 acpi_rsdp_validate(acpi_rsdp_t* rsdp) {
	// Check signature
	if (!strneq(rsdp->signature, "RSD PTR ", 8))
		return 0;

	// Validate checksum
	u8 checksum = 0;
	for (i32 i = 0; i < sizeof(acpi_rsdp_t); ++i)
		checksum += ((u8*)rsdp)[i];
	if (checksum != 0)
		return 0;

	if (rsdp->rev == 2) {
		for (i32 i = sizeof(acpi_rsdp_t); i < sizeof(acpi_rsdp2_t); ++i)
			checksum += ((u8*)rsdp)[i];
		return checksum == 0;
	}
	else
		return rsdp->rev == 0;
}

static
acpi_rsdp_t* acpi_find_rsdp(void) {
	// Read the 16-bit EBDA pointer from 0x040E
	u8* ebda = (u8*)(u32)*(u16*)0x040E;

	// The RSDP is sometimes located in the 1st KiB of the EBDA
	for (u32 i = 0; i < KB(1); i += 16) {
		if (ebda[i] != 'R')
			continue;

		acpi_rsdp_t* rsdp = (acpi_rsdp_t*)&ebda[i];
		if (acpi_rsdp_validate(rsdp))
			return rsdp;
	}

	// Otherwise, it should be located between 0xE0000 and 0xFFFFF
	for (u8* it = (u8*)0xE0000; it < (u8*)0xFFFFF; it += 16) {
		if (*it != 'R')
			continue;

		acpi_rsdp_t* rsdp = (acpi_rsdp_t*)it;
		if (acpi_rsdp_validate(rsdp))
			return rsdp;
	}

	return 0;
}

static
u8 acpi_validate_sdt(acpi_sdt_header_t* sdt) {
	u32 len = sdt->len;
	u8 checksum = 0;

	for (u32 i = 0; i < len; ++i)
		checksum += ((u8*)sdt)[i];
	return checksum == 0;
}

static acpi_rsdp_t* rsdp = 0;
static acpi_rsdt_t* rsdt = 0;

void acpi_initialize(void) {
	dbg_puts("\nInitializing ACPI...\n");
	rsdp = acpi_find_rsdp();
	if (!rsdp) {
		dbg_puts(DBG_YLW"No ACPI found\n"DBG_RST);
		return;
	}

	rsdt = (acpi_rsdt_t*)rsdp->rsdt_addr;
	dbg_printf(DBG_GRY"RSDP: 0x%p, RSDT: 0x%p\n"DBG_RST, rsdp, rsdt);

	u32 sdt_addr_count = (rsdt->header.len - sizeof(acpi_rsdt_t)) / sizeof(u32);

	for (u32 i = 0; i < sdt_addr_count; ++i) {
		acpi_sdt_header_t* header = (acpi_sdt_header_t*)rsdt->sdt_addr_table[i];

		// Make a null-terminated copy of the table signature
		u8 signature[5];
		mcpy8(signature, header->signature, 4);
		signature[4] = 0;

		dbg_printf(DBG_GRY"%s: 0x%p\n"DBG_RST, signature, header);
		if (!acpi_validate_sdt(header)) {
			dbg_printf(DBG_YLW"Table '%s' failed to validate\n"DBG_RST, signature);
			continue;
		}

		if (strneq(signature, "APIC", 4)) {
			acpi_madt_t* madt = (acpi_madt_t*)header;

			u32 active_cpu_count = 0, inactive_cpu_count = 0;
			void* lapic = (void*)madt->lapic_addr, *ioapic = null;
			u8* it = madt->intr_devs, *end = it + header->len - sizeof(acpi_madt_t);

			// Iterate over PIC records, adding the 'length' entry after each one
			for (; it < end; it += it[1]) {
				switch (it[0]) {
				case 0: {
					u32 flags = *((u32*)&it[4]);
					if (flags & 1) // CPU Enabled bit
						++active_cpu_count;
					else if (flags & 2) // Online Capable bit
						++inactive_cpu_count;
				}	break;
				case 1: ioapic = (void*) *((u32*)&it[4]); break;
				case 5: lapic = (void*) *((u32*)&it[4]); break;
				}
			}
			u32 cpu_count = active_cpu_count + inactive_cpu_count;
			dbg_printf(DBG_GRY"CPUs: %i/%i\nLAPIC: 0x%p, IOAPIC: 0x%p\n"DBG_RST, active_cpu_count, cpu_count, lapic, ioapic);
		}
	}
}




