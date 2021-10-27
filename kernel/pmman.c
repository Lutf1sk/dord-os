#include <pmman.h>
#include <memory.h>
#include <debug_io.h>

pmman_map_t pmman_kernel_map; // 4K blocks covering the lowest 4MiB

#define KMAP_SIZE MB(4)

static INLINE
void pmman_set_block(u32* map, usz block) {
	usz index = (block >> 5) & 0x1F;
	usz bit = block & 0x1F;
	map[index] |= ((u32)1 << bit);
}

static INLINE
void pmman_clear_block(u32* map, usz block) {
	usz index = (block >> 5) & 0x1F;
	usz bit = block & 0x1F;
	map[index] &= ~((u32)1 << bit);
}

static INLINE
u8 pmman_get_block(u32* map, usz block) {
	usz index = (block >> 5) & 0x1F;
	usz bit = block & 0x1F;
	return (map[index] >> bit) & 1;
}

void pmman_mark_range(pmman_map_t* map, usz start, usz size) {
	usz end = start + size;
	for (usz i = start; i < end; ++i)
		pmman_set_block(map->bits, i);
}

void pmman_clear_range(pmman_map_t* map, usz start, usz size) {
	usz end = start + size;
	for (usz i = start; i < end; ++i)
		pmman_clear_block(map->bits, i);
}

void* pmman_alloc(pmman_map_t* map, usz size) {
	usz blocks = pmman_to_blocks(map, size);
	if (blocks > PMMAN_BITS)
		return null;

	usz start = 0;
	for (usz i = 0; i <= PMMAN_BITS - blocks; ++i) {
		start = i;
		while (i < PMMAN_BITS && !pmman_get_block(map->bits, i)) {
			if (++i - start == blocks)
				goto found;
		}
	}

	return null;

found:
	dbg_printf("allocated %ud -> %ud (%ud blocks)\n", start, start + blocks, blocks);
	pmman_mark_range(map, start, blocks);
	return (void*) ((start << map->block_size) + map->base);
}

void* pmman_free(pmman_map_t* map, void* addr, usz size) {

}


void pmman_print_map(pmman_map_t* map) {
	for (usz i = 0; i < 32; ++i) {
		for (usz j = 0; j < 32; ++j) {
			dbg_puts(((map->bits[i] >> j) & 1) ? "#" : ".");
		}
		if (i & 1)
			dbg_puts("\n");
	}
}


void pmman_initialize(memmap_t* memmap, u32 memmap_count) {
	dbg_puts("\nInitializing physical memory manager\n");
	pmman_map_t* map = &pmman_kernel_map;

	mset32(map->bits, 0, sizeof(map->bits));
	map->block_size = PMMAN_BLOCK_SIZE_4K;
	map->base = 0;

	// Sort memory maps
	for (usz pass = 0; pass < memmap_count - 1; ++pass) {
		usz min_index = 0;
		u64 min_val = UINT64_MAX;

		// Find smallest base in unsorted subarray
		for (usz i = pass; i < memmap_count; ++i) {
			if (memmap[i].base >= min_val)
				continue;
			min_index = i;
			min_val = memmap[i].base;
		}

		// Swap smallest base with first element
		if (min_index == pass)
			continue;
		memmap_t tmp = memmap[min_index];
		memmap[min_index] = memmap[pass];
		memmap[pass] = tmp;
	}

	// Mark reserved areas
	for (usz i = 0; i < memmap_count; ++i) {
		u64 base = memmap[i].base, len = memmap[i].len;
		dbg_printf(DBG_GRY"%s 0x%hq -> 0x%hq (%uq Bytes)\n"DBG_RST, memmap_type_str(memmap[i].type), base, base + len, len);

		if (memmap[i].type == MEMMAP_USABLE || base > KMAP_SIZE)
			continue;

		if (base + len > KMAP_SIZE)
			len -= base + len - KMAP_SIZE;

		pmman_mark_range(map, pmman_to_block(map, (void*)(usz)base), pmman_to_blocks(map, len));
	}
}

