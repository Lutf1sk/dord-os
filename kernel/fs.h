#ifndef FS_H
#define FS_H 1

#include <err.h>

typedef struct fs fs_t;

typedef err_t (*fs_fread_callback_t)(fs_t* usr, char* path, void* data, usz* out_size);
typedef err_t (*fs_fwrite_callback_t)(fs_t* usr, char* path, void* data, usz size);

typedef
struct fs {
	fs_fread_callback_t fread;
	fs_fwrite_callback_t fwrite;
} fs_t;

extern fs_t* rootfs;

static INLINE
err_t fread(char* path, void* data, usz* out_size) {
	return rootfs->fread(rootfs, path, data, out_size);
}

static INLINE
err_t fwrite(char* path, void* data, usz size) {
	return rootfs->fwrite(rootfs, path, data, size);
}

#endif
