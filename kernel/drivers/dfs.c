#include <memory.h>

#include <drivers/dfs.h>

err_t dfs_init(dfs_t* fs, drive_t* drive) {
	fs->interf.fread = (fs_fread_callback_t)dfs_fread;
	fs->drive = drive;
	return OK;
}

err_t dfs_fread(dfs_t* fs, char* path, void* out_data, usz* out_size) {
	u64 lba = 1;
	char buf[512];
	dfs_file_header_t* fh = (dfs_file_header_t*)buf;
	do {
		drive_read(fs->drive, lba, 1, buf);

		if (strneq(path, fh->name, 32)) {
			usz filesz = fh->sectors * 512;
			*out_size = filesz;
			if (!out_data)
				return OK;
			return drive_read(fs->drive, lba + 1, fh->sectors, out_data);
		}

		lba = fh->next_lba;
	} while (lba);

	return ERR_NOTFOUND;
}

