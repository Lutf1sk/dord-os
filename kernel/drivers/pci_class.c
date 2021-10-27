#include <drivers/pci.h>

char* pci_get_class_str(u8 class_code, u8 subclass, u8 prog_if) {
	switch (class_code) {
		case 0x00:
			switch (subclass) {
				case 0x01: return "VGA Compatible Device";
				default: return "Unclassified Device";
			}
		case 0x01:
			switch (subclass) {
				case 0x00: return "SCSI Bus Controller";
				case 0x01: return "IDE Controller";
				case 0x02: return "Floppy Disk Controller";
				case 0x03: return "IPI Bus Controller";
				case 0x04: return "RAID Controller";
				case 0x05: return "ATA Controller";
				case 0x06: return "Serial ATA Controller";
				case 0x07: return "Serial Attached SCSI Controller";
				case 0x08: return "Non Volatile Memory Controller";
				default: return "Unknown Mass Storage Controller";
			}
		case 0x02:
			switch (subclass) {
				case 0x00: return "Ethernet Controller";
				case 0x01: return "Token Ring Controller";
				case 0x02: return "FDDI Controller";
				case 0x03: return "ATM Controller";
				case 0x04: return "ISDN Controller";
				case 0x05: return "WorldFip Controller";
				case 0x06: return "PICMG 2.14 Multi Computing";
				case 0x07: return "Infiniband Controller";
				case 0x08: return "Fabric Controller";
				default: return "Unknown Network controller";
			}
		case 0x03:
			switch (subclass) {
				case 0x00: return "VGA Controller";
				case 0x01: return "XGA Controller";
				case 0x02: return "3D Controller";
				default: return "Unknown Display controller";
			}
		case 0x04:
			switch (subclass) {
				case 0x00: return "Multimedia Video Controller";
				case 0x01: return "Multimedia Audio Controller";
				case 0x02: return "Computer Telephony Device";
				case 0x03: return "Audio Device";
				default: return "Unknown Multimedia controller";
			}
		case 0x05:
			switch (subclass) {
				case 0x00: return "RAM Controller";
				case 0x01: return "Flash Controller";
				default: return "Unknown Memory controller";
			}
		case 0x06:
			switch (subclass) {
				case 0x00: return "Host Bridge";
				case 0x01: return "ISA Bridge";
				case 0x02: return "EISA Bridge";
				case 0x03: return "MCA Bridge";
				case 0x04: return "PCI to PCI Bridge";
				case 0x05: return "PCMCIA Bridge";
				case 0x06: return "NuBus Bridge";
				case 0x07: return "CardBus Bridge";
				case 0x08: return "RACEway Bridge";
				case 0x09: return "PCI to PCI Bridge";
				case 0x0A: return "InfiniVand to PCI Bridge";
				default: return "Unknown Bridge";
			}
		case 0x07:
			switch (subclass) {
				case 0x00: return "Serial Controller";
				case 0x01: return "Parallel Controller";
				case 0x02: return "Multiport Serial Controller";
				case 0x03: return "Modem";
				case 0x04: return "IEEE 488.1/2 Controller";
				case 0x05: return "Smart Card";
				default: return "Unknown Communication Controller";
			}

		case 0x08:
			switch (subclass) {
				case 0x00: return "PIC";
				case 0x01: return "DMA Controller";
				case 0x02: return "Timer";
				case 0x03: return "RTC Controller";
				case 0x04: return "PCI Hot Plug Controller";
				case 0x05: return "SD Host Controller";
				case 0x06: return "IOMMU";
				default: return "Unknown Base System Peripheral";
			}

		case 0x09:
			switch (subclass) {
				case 0x00: return "Keyboard Controller";
				case 0x01: return "Digitizer Pen";
				case 0x02: return "Mouse Controller";
				case 0x03: return "Scanner Controller";
				case 0x04: return "Gameport Controller";
				default: return "Unknown Input device controller";
			}

		case 0x0A:
			switch (subclass) {
				case 0x00: return "Generic Docking Station";
				default: return "Unknown Docking station";
			}

		case 0x0B:
			switch (subclass) {
				case 0x00: return "386";
				case 0x01: return "486";
				case 0x02: return "Pentium";
				case 0x03: return "Pentium Pro";
				case 0x10: return "Alpha";
				case 0x20: return "PowerPC";
				case 0x30: return "MIPS";
				case 0x40: return "Coprocessor";
				default: return "Unknown Processor";
			}

		case 0x0C:

			switch (subclass) {
				case 0x00: return "FireWire Controller";
				case 0x01: return "ACCESS Bus";
				case 0x02: return "SSA";
				case 0x03: return "USB Controller";
				case 0x04: return "Fibre Channel";
				case 0x05: return "SMBus";
				case 0x06: return "InfiniBand";
				case 0x07: return "IPMI Interface";
				case 0x08: return "SERCOS Interface";
				case 0x09: return "CANbus";
				default: return "Unknown Serial Bus Controller";
			}

		case 0x0D:
			switch (subclass) {
				case 0x00: return "iRDA Controller";
				case 0x01: return "IR Controller";
				case 0x10: return "RF Controller";
				case 0x11: return "Bluetooth Controller";
				case 0x12: return "Broadband Controller";
				case 0x20: return "Ethernet 802.1a Controller";
				case 0x21: return "Ethernet 802.1b Controller";
				default: return "Unknown Wireless controller";
			}

		case 0x0E:
			switch (subclass) {
				case 0x00: return "I20";
				default: return "Unknown Intelligent Controller";
			}

		case 0x0F:
			switch (subclass) {
				case 0x01: return "Satellite TV Controller";
				case 0x02: return "Satellite Audio Controller";
				case 0x03: return "Satellite Voice Controller";
				case 0x04: return "Satellite Data Controller";
				default: return "Unknown Satellite Communications Controller";
			}

		case 0x10:

			switch (subclass) {
				case 0x00: return "Network and Computing Encryption Controller";
				case 0x10: return "Entertainment Encryption Controller";
				default: return "Unknown Encryption controller";
			}
		case 0x11:
			switch (subclass) {
				case 0x00: return "DPIO Modules";
				case 0x01: return "Performance Counters";
				case 0x10: return "Communication Sychronizer";
				case 0x20: return "Signal Processing Manager";
				default: return "Unknown Signal processing controller";
			}

		case 0x12: return "Processing Accelerators";
		case 0x13: return "Non-Essential Instrumentation";
		case 0x40: return "Coprocessor";
		case 0xFF: return "Unassigned Class";
		default: return "Unknown Device";
	}
}
