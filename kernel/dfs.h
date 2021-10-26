#ifndef DFS_H
#define DFS_H 1

#include "ide.h"

typedef
struct PACKED dfs_file_header {
	u64 sectors, next_lba;
	char name[32];
} dfs_file_header_t;

typedef
struct dfs_it {
	ide_drive_t* drive;
	u64 lba, sectors, next_lba;
	char name[32];
} dfs_it_t;

dfs_it_t dfs_it_begin(ide_drive_t* drive);

u8 dfs_iterate(dfs_it_t* it);

void dfs_read(void* dst, dfs_it_t* it);

#endif
