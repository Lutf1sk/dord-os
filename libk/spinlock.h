#ifndef SPINLOCK_H
#define SPINLOCK_H

#include "common.h"

typedef
struct SpinLock {
	u8 locked;
} SpinLock;

static inline INLINE
SpinLock make_spinlock() { return (SpinLock) { 0 }; }

void spinlock_lock(SpinLock* lock);
void spinlock_unlock(SpinLock* lock);

#endif
