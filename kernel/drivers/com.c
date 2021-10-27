#include <asm.h>

#include <drivers/com.h>

// The current setup with QEMU redirects
// COM1 output to the host terminal, this
// is the only reason this is implemented
void com_write_b(u16 port, u8 data) {
	while (!(inb(port + 5) & 0x20))
		;

	outb(port, data);
}
