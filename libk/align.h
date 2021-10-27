#ifndef ALIGN_H
#define ALIGN_H 1

#include <common.h>

#define ALIGN_DEFAULT (sizeof(usz) << 1)

static INLINE
u8 is_pow2(usz n) {
	return !(n & (n - 1));
}

static INLINE
usz pad(usz size, usz align) {
	usz align_mask = (align - 1);
	usz n = size & align_mask;
	n ^= align_mask;
	return n + 1;
}

static INLINE
usz align_default_fwd(usz val) {
	return (val + (ALIGN_DEFAULT - 1)) & ~(ALIGN_DEFAULT - 1);
}

static INLINE
usz align_default_bwd(usz val) {
	return val & ~(ALIGN_DEFAULT - 1);
}

static INLINE
usz align_fwd(usz val, usz align) {
	usz align_mask = align - 1 ;
	return (val + align_mask) & ~align_mask;
}

static INLINE
usz align_bwd(usz val, usz align) {
	return val & ~(align - 1);
}

#endif
