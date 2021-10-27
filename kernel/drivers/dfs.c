#include <memory.h>

#include <drivers/dfs.h>

dfs_it_t dfs_it_begin(ide_drive_t* drive) {
	dfs_it_t it;
	it.drive = drive;
	it.next_lba = 1;
	it.name[0] = 0;
	return it;
}

u8 dfs_iterate(dfs_it_t* it) {
	if (!it->next_lba)
		return 0;

	char buf[512];
	dfs_file_header_t* fh = (dfs_file_header_t*)buf;

	it->lba = it->next_lba;

	ide_read_drive(it->drive, buf, it->lba, 1);
	it->next_lba = fh->next_lba;
	mcpy8(it->name, fh->name, 32);
	it->sectors = fh->sectors;

	return 1;
}

void dfs_read(void* dst, dfs_it_t* it) {
	ide_read_drive(it->drive, dst, it->lba + 1, it->sectors);
}

