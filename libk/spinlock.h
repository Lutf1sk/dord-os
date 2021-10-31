#ifndef SPINLOCK_H
#define SPINLOCK_H

#include "common.h"

typedef
struct slock_t {
	volatile u8 locked;
} slock_t;

#define SLOCK_INIT() {0}
#define SLOCK() ((slock_t)SLOCK_INIT)

void spinlock_lock(slock_t* lock);
void spinlock_release(slock_t* lock);

#endif
