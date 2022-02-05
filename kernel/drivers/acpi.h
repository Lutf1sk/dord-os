#ifndef ACPI_H
#define ACPI_H 1

#include <common.h>

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


// 64-bit Generic Address Structure
typedef
struct PACKED acpi_gas {
	u8 addr_space;
	u8 bit_width;
	u8 bit_offs;
	u8 access_size;
	u64 access;
} acpi_gas_t;


// Fixed ACPI Description Table
typedef
struct PACKED acpi_fadt {
	acpi_sdt_header_t header;
	u32 firmware_ctrl;
	u32 dsdt;
	u8 reserved;
	u8 pref_power_management_prof;
	u16 sci_intr;
	u32 smi_cmd_port;
	u8 acpi_enable;
	u8 acpi_disable;
	u8 s4bios_req;
	u8 pstate_ctrl;
	u32 pm1a_evnt_block;
	u32 pm1b_evnt_block;
	u32 pm1a_ctrl_block;
	u32 pm1b_ctrl_block;
	u32 pm2_ctrl_block;
	u32 pm_timer_block;
	u32 gpe0_block;
	u32 gpe1_block;
	u8 pm1_evnt_len;
	u8 pm1_ctrl_len;
	u8 pm2_ctrl_len;
	u8 pm_timer_len;
	u8 gpe0_len;
	u8 gpe1_len;
	u8 gpe1_base;
	u8 cstate_ctrl;
	u16 worst_c2_latency;
	u16 worst_c3_latency;
	u16 flush_size;
	u16 flush_stride;
	u8 duty_offs;
	u8 duty_width;
	u8 day_alarm;
	u8 month_alarm;
	u8 century;

	// Reserved in ACPI 1.0, used in ACPI 2.0+
	u16 boot_arch_flags;
	u8 reserved2; // Is this in the wrong order?
	u32 flags;
	acpi_gas_t reset_reg;
	u8 reset_val;
	u8 reserved3[3];

	// 64-bit pointers for ACPI 2.0+
	u64 x_firmware_ctrl;
	u64 x_dsdt;
	acpi_gas_t x_pm1a_evnt_block;
	acpi_gas_t x_pm1b_evnt_block;
	acpi_gas_t x_pm1a_ctrl_block;
	acpi_gas_t x_pm1b_ctrl_block;
	acpi_gas_t x_pm2_ctrl_block;
	acpi_gas_t x_pm_timer_block;
	acpi_gas_t x_gpe0_block;
	acpi_gas_t x_gpe1_block;
} acpi_fadt_t;

extern void* acpi_lapic;
extern void* acpi_ioapic;

void acpi_initialize(void);

void acpi_enable(void);
void acpi_disable(void);

#endif
