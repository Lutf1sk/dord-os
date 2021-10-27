#ifndef PCI_H
#define PCI_H

#include <common.h>

#define PCI_MAX_BUS 0xFF
#define PCI_MAX_DEVICE 0x10
#define PCI_MAX_FUNC 0x04

#define PCI_CONFIG_ADDR 0x0CF8
#define PCI_CONFIG_DATA 0x0CFC


#define PCI_HEADER_MAX_SIZE 0x44

#define PCI_HEADER_TYPE_0 0x00
#define PCI_HEADER_TYPE_1 0x01
#define PCI_HEADER_TYPE_2 0x02

// Common header registers
#define PCI_HC_VENDOR_ID	0x00
#define PCI_HC_DEVICE_ID	0x02

#define PCI_HC_COMMAND	0x04
#define PCI_HC_STATUS	0x06

#define PCI_HC_REV_ID	0x08
#define PCI_HC_PROG_IF	0x09
#define PCI_HC_SUBCLASS	0x0A
#define PCI_HC_CLASS	0x0B

#define PCI_HC_CACHE_LINE_SIZE	0x0C
#define PCI_HC_LATENCY_TIMER	0x0D
#define PCI_HC_HEADER_TYPE		0x0E
#define PCI_HC_BIST				0x0F


// Header 0 registers
#define PCI_H0_BAR0	0x10
#define PCI_H0_BAR1	0x14
#define PCI_H0_BAR2	0x18
#define PCI_H0_BAR3	0x1C
#define PCI_H0_BAR4	0x20
#define PCI_H0_BAR5	0x24

#define PCI_H0_CCS_PTR	0x28

#define PCI_H0_SUBSYS_VENDOR_ID	0x2C
#define PCI_H0_SUBSYS_ID		0x2E

#define PCI_H0_EXPANSION_ROM_BASE_ADDR	0x30

#define PCI_H0_CAPABILITIES_PTR	0x34

#define PCI_H0_INTERRUPT_LINE	0x3C
#define PCI_H0_INTERRUPT_PIN	0x3D
#define PCI_H0_MIN_GRANT		0x3E
#define PCI_H0_MAX_LATENCY		0x3E

typedef
struct pci_dev {
	u8 bus, dev, func;
	char* class_str;
	u8 reg_data[PCI_HEADER_MAX_SIZE];
} pci_dev_t;


pci_dev_t* pci_enumerate(u32* out_count);
void pci_read_registers(pci_dev_t* device);

char* pci_get_class_str(u8 class_code, u8 subclass, u8 prog_if);

static inline INLINE
u32 pci_reg32(pci_dev_t* device, u32 offset) {
	return *(u32*)(&device->reg_data[offset]);
}

#endif
