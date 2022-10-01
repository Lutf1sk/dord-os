#ifndef DFS_H
#define DFS_H 1

#include <drive.h>
#include <fs.h>

typedef
struct PACKED dfs_file_header {
	u64 sectors, next_lba;
	char name[32];
} dfs_file_header_t;

typedef
struct dfs {
	fs_t interf;
	drive_t* drive;
} dfs_t;

err_t dfs_init(dfs_t* fs, drive_t* drive);
err_t dfs_fread(dfs_t* fs, char* path, void* out_data, usz* out_size);

#endif
