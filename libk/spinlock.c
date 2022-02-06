#include "spinlock.h"

static inline INLINE
u8 atomic_test_and_set(u8 volatile* val) {
	u8 cf;
	__asm__ volatile ("lock bts DWORD PTR [%1], 0\n" : "=@ccc"(cf) : "r"(val));
	return cf;
}

void spinlock_lock(slock_t* lock) {
	while (atomic_test_and_set(&lock->locked))
		;
}

void spinlock_release(slock_t* lock) {
	lock->locked = 0;
}
