#ifndef IDE_H
#define IDE_H

#include "common.h"

// Status bits
#define ATA_SR_BSY	   0x80	   // Busy
#define ATA_SR_DRDY	   0x40	   // Drive ready
#define ATA_SR_DF	   0x20	   // Drive write fault
#define ATA_SR_DSC	   0x10	   // Drive seek complete
#define ATA_SR_DRQ	   0x08	   // Data request ready
#define ATA_SR_CORR	   0x04	   // Corrected data
#define ATA_SR_IDX	   0x02	   // Index
#define ATA_SR_ERR	   0x01	   // Error

// Error bits
#define ATA_ER_BBK		0x80	// Bad block
#define ATA_ER_UNC		0x40	// Uncorrectable data
#define ATA_ER_MC		0x20	// Media changed
#define ATA_ER_IDNF		0x10	// ID mark not found
#define ATA_ER_MCR		0x08	// Media change request
#define ATA_ER_ABRT		0x04	// Command aborted
#define ATA_ER_TK0NF	0x02	// Track 0 not found
#define ATA_ER_AMNF		0x01	// No address mark

// ATA commands
#define ATA_CMD_READ_PIO		  0x20
#define ATA_CMD_READ_PIO_EXT	  0x24
#define ATA_CMD_READ_DMA		  0xC8
#define ATA_CMD_READ_DMA_EXT	  0x25
#define ATA_CMD_WRITE_PIO		  0x30
#define ATA_CMD_WRITE_PIO_EXT	  0x34
#define ATA_CMD_WRITE_DMA		  0xCA
#define ATA_CMD_WRITE_DMA_EXT	  0x35
#define ATA_CMD_CACHE_FLUSH		  0xE7
#define ATA_CMD_CACHE_FLUSH_EXT	  0xEA
#define ATA_CMD_PACKET			  0xA0
#define ATA_CMD_IDENTIFY_PACKET	  0xA1
#define ATA_CMD_IDENTIFY		  0xEC

// ATAPI commands
#define		 ATAPI_CMD_READ		  0xA8
#define		 ATAPI_CMD_EJECT	  0x1B

// Identification space
#define ATA_IDENT_DEVICETYPE   0
#define ATA_IDENT_CYLINDERS	   2
#define ATA_IDENT_HEADS		   6
#define ATA_IDENT_SECTORS	   12
#define ATA_IDENT_SERIAL	   20
#define ATA_IDENT_MODEL		   54
#define ATA_IDENT_CAPABILITIES 98
#define ATA_IDENT_FIELDVALID   106
#define ATA_IDENT_MAX_LBA	   120
#define ATA_IDENT_COMMANDSETS  164
#define ATA_IDENT_MAX_LBA_EXT  200

// Device types
#define IDE_ATA		0x00
#define IDE_ATAPI	0x01
#define IDE_SATA	0x02
#define IDE_SATAPI	0x03

// Buses
#define ATA_MASTER	   0x00
#define ATA_SLAVE	   0x01

// Base registers
#define ATA_REG_DATA		0x00
#define ATA_REG_ERROR		0x01
#define ATA_REG_FEATURES	0x01
#define ATA_REG_SECCOUNT	0x02
#define ATA_REG_LBA0		0x03
#define ATA_REG_LBA1		0x04
#define ATA_REG_LBA2		0x05
#define ATA_REG_DEVSEL		0x06
#define ATA_REG_STATUS		0x07
#define ATA_REG_COMMAND		0x07

// Control registers
#define ATA_REG_ALTSTATUS	0x00
#define ATA_REG_DEVCTRL		0x00
#define ATA_REG_DEVADDR		0x00

// Channels
#define		 ATA_PRIMARY	  0x00
#define		 ATA_SECONDARY	  0x01

// Directions
#define		 ATA_READ	   0x00
#define		 ATA_WRITE	   0x01

// Default ports
#define IDE_ATA_STANDARD_PRIMARY_PORT			0x1F0
#define IDE_ATA_STANDARD_PRIMARY_CONTROL_PORT	0x3F6
#define IDE_ATA_STANDARD_SECONDARY_PORT			0x170
#define IDE_ATA_STANDARD_SECONDARY_CONTROL_PORT	0x376

typedef
struct ide_drive {
	u8 channel;
	u8 bus;
	u8 type;
	u8 name[41];
	u16 signature;
	u16 capabilities;
	u32 cmd_sets;
	u32 size;
} ide_drive_t;

typedef
struct ide_channel {
	u16 base_port;
	u16 ctrl_port;
	u16 bmide_port;
} ide_channel_t;

ide_drive_t* ide_initialize(u32* out_drive_count, u32 bar0, u32 bar1, u32 bar2, u32 bar3, u32 bar4);

void ide_read_drive(ide_drive_t* drive, void* dst, u64 lba, u16 sector_count);

#endif
