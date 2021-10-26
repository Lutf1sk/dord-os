#include "ide.h"

#include "asm.h"
#include "debug_io.h"
#include <memory.h>

static ide_drive_t ide_drives[4];
static u32 ide_drive_count = 0;

static ide_channel_t ide_channels[2];

static
void ide_wait_bsy(u16 port) {
	while (inb(port) & ATA_SR_BSY) // Wait while BSY bit is set
		;
}

static
void ide_await_drq(u16 port) {
	while (!(inb(port) & ATA_SR_DRQ)) // Wait while DRQ bit is clear
		;
}

static
void ide_delay(u16 port) {
	for (u32 i = 0; i < 4; ++i)
		inb(port);
}

static
void ide_check_and_add_drive(u8 channel, u8 bus) {
	u16 base = ide_channels[channel].base_port, ctrl = ide_channels[channel].ctrl_port, type = IDE_ATA;

	// Select drive
	outb(base + ATA_REG_DEVSEL, 0xA0 | bus << 4);
	ide_delay(ctrl);

	// Send ITENTIFY
	outb(base + ATA_REG_LBA0, 0);
	outb(base + ATA_REG_LBA1, 0);
	outb(base + ATA_REG_LBA2, 0);
	outb(base + ATA_REG_COMMAND, ATA_CMD_IDENTIFY);
	ide_delay(ctrl);

	// Return if status is zero, the port has no drive connected
	if (!inb(base + ATA_REG_STATUS))
		return;
	ide_wait_bsy(base + ATA_REG_STATUS);

	// Check lba registers to detect non-ATA drives
	u8 lba1 = inb(base + ATA_REG_LBA1), lba2 = inb(base + ATA_REG_LBA2);
	if ((lba1 == 0x14 && lba2 == 0xEB) || (lba1 == 0x69 && lba2 == 0x96))
		goto atapi;
	else if (lba1 || lba2)
		return;

	// Poll status register until ERR or DRQ sets
	while (1) {
		u8 status = inb(base + ATA_REG_STATUS);
		if (status & ATA_SR_ERR)
			return;
		if (status & ATA_SR_DRQ)
			goto found;
	}

atapi:
	// If the drive was ATAPI, it needs ATAPI-equivalent of IDENTIFY command,
	// this should probably have proper error checking
	type = IDE_ATAPI;
	outb(base + ATA_REG_COMMAND, ATA_CMD_IDENTIFY_PACKET);
	ide_delay(ctrl);
	ide_await_drq(base + ATA_REG_STATUS);

found:
	static u8 id_buf[512] = {0};
	u16* it = (u16*)id_buf;

	for (int i = 0; i < 256; i++)
		*it++ = inw(base + ATA_REG_DATA);

	// Fill new 'drive' structure with collected info
	ide_drive_t* drive = &ide_drives[ide_drive_count++];
	drive->type = type;
	drive->channel = channel;
	drive->bus = bus;
	drive->signature = *((u16*)(id_buf + ATA_IDENT_DEVICETYPE));
	drive->capabilities = *((u16*)(id_buf + ATA_IDENT_CAPABILITIES));
	drive->cmd_sets = *((u32*)(id_buf + ATA_IDENT_COMMANDSETS));

	// 48bit addressing
	if (drive->cmd_sets & (1 << 26))
		drive->size = *((u32*)(id_buf + ATA_IDENT_MAX_LBA_EXT));

	// CHS or 28bit addressing (currently unsupported)
	else
		drive->size = *((u32*)(id_buf + ATA_IDENT_MAX_LBA));

	for (u32 i = 0; i < 40; i += 2) {
		drive->name[i] = id_buf[ATA_IDENT_MODEL + i + 1];
		drive->name[i + 1] = id_buf[ATA_IDENT_MODEL + i];
	}

 	// Trim right whitespace of name
	for (u32 i = 40 - 1; i; --i) {
		if (drive->name[i] != ' ') {
			drive->name[i + 1] = 0;
			break;
		}
	}
}

static
void ide_print_error(u8 err) {
	dbg_printf("ATA Error:\n");
	if (err & ATA_ER_BBK)	dbg_printf("- Bad block\n");
	if (err & ATA_ER_UNC)	dbg_printf("- Uncorrectable data\n");
	if (err & ATA_ER_MC)	dbg_printf("- Media changed\n");
	if (err & ATA_ER_IDNF)	dbg_printf("- ID mark not found\n");
	if (err & ATA_ER_MCR)	dbg_printf("- Media change request\n");
	if (err & ATA_ER_ABRT)	dbg_printf("- Command aborted\n");
	if (err & ATA_ER_TK0NF)	dbg_printf("- Track 0 not found\n");
	if (err & ATA_ER_AMNF)	dbg_printf("- No address mark\n");
}

ide_drive_t* ide_initialize(u32* out_drive_count, u32 bar0, u32 bar1, u32 bar2, u32 bar3, u32 bar4) {
	if (!bar0 || bar0 == 1)
		bar0 = IDE_ATA_STANDARD_PRIMARY_PORT;
	if (!bar1 || bar1 == 1)
		bar1 = IDE_ATA_STANDARD_PRIMARY_CONTROL_PORT;
	if (!bar2 || bar2 == 1)
		bar2 = IDE_ATA_STANDARD_SECONDARY_PORT;
	if (!bar3 || bar3 == 1)
		bar3 = IDE_ATA_STANDARD_SECONDARY_CONTROL_PORT;

	ide_channels[ATA_PRIMARY].base_port = bar0;
	ide_channels[ATA_PRIMARY].ctrl_port = bar1;
	ide_channels[ATA_PRIMARY].bmide_port = bar4;

	ide_channels[ATA_SECONDARY].base_port = bar2;
	ide_channels[ATA_SECONDARY].ctrl_port = bar3;
	ide_channels[ATA_SECONDARY].bmide_port = bar4 + 8;

	// Disable interrupts
	outb(bar1 + ATA_REG_DEVCTRL, 2);
	outb(bar3 + ATA_REG_DEVCTRL, 2);

	// Detect drives
	ide_check_and_add_drive(ATA_PRIMARY, ATA_MASTER);
	ide_check_and_add_drive(ATA_PRIMARY, ATA_SLAVE);
	ide_check_and_add_drive(ATA_SECONDARY, ATA_MASTER);
	ide_check_and_add_drive(ATA_SECONDARY, ATA_SLAVE);

	*out_drive_count = ide_drive_count;
	return ide_drives;
}

void ide_read_drive(ide_drive_t* drive, void* dst, u64 lba, u16 sector_count) {
	u16 base = ide_channels[drive->channel].base_port, ctrl = ide_channels[drive->channel].ctrl_port;

	// Select drive
	ide_wait_bsy(base + ATA_REG_STATUS);
	outb(base + ATA_REG_DEVSEL, 0x40 | (drive->bus << 4));
	ide_delay(ctrl);

	// High values
	outb(base + ATA_REG_SECCOUNT, sector_count >> 8);
	outb(base + ATA_REG_LBA0, lba >> 24);
	outb(base + ATA_REG_LBA1, lba >> 32);
	outb(base + ATA_REG_LBA2, lba >> 40);

	// Low values
	outb(base + ATA_REG_SECCOUNT, sector_count);
	outb(base + ATA_REG_LBA0, lba);
	outb(base + ATA_REG_LBA1, lba >> 8);
	outb(base + ATA_REG_LBA2, lba >> 16);

	// Extended PIO read
	outb(base + ATA_REG_COMMAND, ATA_CMD_READ_PIO_EXT);
	ide_delay(ctrl);
	ide_wait_bsy(base + ATA_REG_STATUS);

	u8 status = inb(base + ATA_REG_STATUS);
	if (status & ATA_SR_ERR) {
		u8 err = inb(base + ATA_REG_ERROR);
		ide_print_error(err);
		return;
	}
	if (status & ATA_SR_DF)
		dbg_printf("ATA_CMD_READ_PIO_EXT: ATA Drive fault\n");

	u16* it = dst;
	for (u32 i = 0; i < sector_count; ++i) {
		ide_wait_bsy(base + ATA_REG_STATUS);
		ide_await_drq(base + ATA_REG_STATUS); // Wait for DRQ to signal that data is available

		for (u32 j = 0; j < 256; ++j)
			*it++ = inw(base + ATA_REG_DATA);
	}
	ide_wait_bsy(base + ATA_REG_STATUS);

// 	// Flush cache
// 	outb(base + ATA_REG_COMMAND, ATA_CMD_CACHE_FLUSH_EXT);
// 	ide_delay(ctrl);
// 	ide_wait_bsy(base + ATA_REG_STATUS);
}

