#include "spinlock.h"

static inline INLINE
u8 atomic_test_and_set(u8* val) {
	u8 cf;
	__asm__ volatile ("lock bts DWORD PTR [%1], 0\n" : "=@ccc"(cf) : "r"(val));
	return cf;
}

void spinlock_lock(SpinLock* lock) {
	while (atomic_test_and_set(&lock->locked))
		;
}

void spinlock_unlock(SpinLock* lock) {
	lock->locked = 0;
}
