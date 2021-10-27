#include <debug_io.h>
#include <asm.h>

#include <drivers/pci.h>

static pci_dev_t pci_devices[128];

static
u32 make_addr(u8 bus, u8 device, u8 func, u8 offset) {
	return ((u32)0b1 << 31) | ((u32)bus << 16) | (((u32)device & 0xF) << 11) | (((u32)func & 0xF) << 8) | (offset & 0xFC);
}

static
u32 read_reg(u8 bus, u8 device, u8 func, u8 offset) {
	outl(PCI_CONFIG_ADDR, make_addr(bus, device, func, offset));
	return inl(PCI_CONFIG_DATA) >> ((offset & 2) * 8);
}

void pci_read_registers(pci_dev_t* device) {
	u32* it = (u32*)device->reg_data;
	for (u32 i = 0; i < 20; i++)
		*it++ = read_reg(device->bus, device->dev, device->func, i << 2);

	device->class_str = pci_get_class_str(device->reg_data[PCI_HC_CLASS], device->reg_data[PCI_HC_SUBCLASS], device->reg_data[PCI_HC_PROG_IF]);
}

pci_dev_t* pci_enumerate(u32* out_count) {
	dbg_puts("\nEnumerating PCI devices...\n");
	u32 dev_count = 0;

	for (u16 bus = 0; bus < PCI_MAX_BUS; bus++) {
		for (u8 device = 0; device < PCI_MAX_DEVICE; device++) {
			for (u8 func = 0; func < PCI_MAX_FUNC; func++) {
				u16 res = read_reg(bus, device, func, 0);

				if (res != 0xFFFF) {
					pci_dev_t* dev = &pci_devices[dev_count++];
					dev->bus = bus;
					dev->dev = device;
					dev->func = func;

					pci_read_registers(dev);

					dbg_printf(DBG_GRY"%ud:%ud:%ud '%s'\n"DBG_RST, bus, device, func, dev->class_str);
				}
			}
		}
	}

	*out_count = dev_count;
	return pci_devices;
}
