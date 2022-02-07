#include <proc.h>
#include <pmman.h>
#include <debug_io.h>
#include <spinlock.h>
#include <asm.h>

#include <x86/cpuid.h>

#include <drivers/pit.h>

proc_t* proc_current = null;

static proc_t* ready_q = null;
static proc_t* blocked_q = null;

slock_t sched_lock = SLOCK_INIT();

#define PROC_SIZE KB(4)

void panic(char*);

proc_t* proc_create(void* entry, char* name) {
	u8* new = pmman_alloc(&pmman_kernel_map, PROC_SIZE);
	proc_t* proc = (proc_t*)new;
	proc->entry = entry;
	proc->next = null;
	proc->name = name;

	u32* stack = (u32*)(new + KB(4));

	proc->sp = stack;
	proc_init(proc);
	return proc;
}

void proc_register(proc_t* proc) {
	volatile u32 flags = proc_lock();

	proc->next = ready_q;
	ready_q = proc;

	proc_release(flags);
}

void proc_exit(void) {
	volatile u32 flags = proc_lock();

	pmman_free(&pmman_kernel_map, proc_current, PROC_SIZE);
	proc_current = null;

	proc_schedule();

	proc_release(flags);
}

void proc_yield(void) {
	volatile u32 flags = proc_lock();

	proc_current->time_end = pit_systime_msec;

	proc_release(flags);
}

void proc_schedule(void) {
	if (!ready_q)
		return;

	if (proc_current) {
		if (pit_systime_msec < proc_current->time_end)
			return;

		// Add current process to the end of the waiting queue
		proc_t** it = &ready_q;
		while (*it)
			it = &(*it)->next;
		*it = proc_current;
	}

	proc_t* next_proc = ready_q;

	if (ready_q->next)
		ready_q = ready_q->next;

	next_proc->next = null;

// 	dbg_printf("[CPU%ud] Switching to %s (0x%hd)\n", cpu_lapic_id(), next_proc->name, next_proc);
// 	proc_t* it = ready_q;
// 	while (it) {
// 		dbg_printf("Queued: %s(0x%hd)\n", it->name, it);
// 		it = it->next;
// 	}

	proc_switch(next_proc);
}

u32 proc_lock(void) {
	volatile u32 flags = getf();
	cli();
// 	dbg_printf(DBG_YLW"Locking...\n"DBG_RST);
	spinlock_lock(&sched_lock);
// 	dbg_printf(DBG_GRN"Locked\n"DBG_RST);
	return flags;
}

void proc_release(volatile u32 flags) {
// 	dbg_printf(DBG_RED"Unlocking...\n"DBG_RST);
	spinlock_release(&sched_lock);
	setf(flags);
}

