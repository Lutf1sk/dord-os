#ifndef DRIVE_H
#define DRIVE_H 1

#include <err.h>

typedef struct drive drive_t;

typedef err_t (*drive_read_callback_t)(drive_t* usr, u64 lba, u64 sectors, void* out_data);

typedef
struct drive {
	drive_read_callback_t read;
} drive_t;

static INLINE
err_t drive_read(drive_t* drive, u64 lba, u64 sectors, void* out_data) {
	return drive->read(drive, lba, sectors, out_data);
}

#endif
