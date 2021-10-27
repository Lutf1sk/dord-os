#ifndef PMMAN_H
#define PMMAN_H 1

#include <align.h>

#include <drivers/memmap.h>

#define PMMAN_BLOCK_SIZE_4K 12
#define PMMAN_BITS (32*32)

typedef
struct pmman_map {
	usz block_size;
	usz base;
	u32 bits[32];
} pmman_map_t;

extern pmman_map_t pmman_kernel_map;

static INLINE
usz pmman_to_block(pmman_map_t* map, void* addr) {
	return ((usz)addr - map->base) >> map->block_size;
}

static INLINE
usz pmman_to_blocks(pmman_map_t* map, usz len) {
	return align_fwd(len, 1 << map->block_size) >> map->block_size;
}

void pmman_mark_range(pmman_map_t* map, usz base, usz size);
void pmman_clear_range(pmman_map_t* map, usz base, usz size);

void* pmman_alloc(pmman_map_t* map, usz size);
void* pmman_free(pmman_map_t* map, void* addr, usz size);

void pmman_print_map(pmman_map_t* map);

void pmman_initialize(memmap_t* memmap, u32 memmap_count);

#endif
