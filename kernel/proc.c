#include <proc.h>
#include <pmman.h>
#include <debug_io.h>
#include <spinlock.h>
#include <asm.h>

#include <drivers/pit.h>

proc_t* proc_current = null;

// lists
static proc_t* ready_q = null;
static proc_t* blocked_q = null;

slock_t sched_lock = SLOCK_INIT();

#define PROC_SIZE KB(4)

void panic(char*);

proc_t* proc_create(void* entry) {
	u8* new = pmman_alloc(&pmman_kernel_map, PROC_SIZE);
	proc_t* proc = (proc_t*)new;
	proc->entry = entry;
	proc->next = null;

	u32* stack = (u32*)(new + KB(4));

	proc->sp = stack;
	proc_init(proc);
	return proc;
}

void proc_register(proc_t* proc) {
	u32 flags = proc_lock();

	proc->next = ready_q;
	ready_q = proc;

	proc_release(flags);
}

void proc_exit(void) {
	u32 flags = proc_lock();

	pmman_free(&pmman_kernel_map, proc_current, PROC_SIZE);
	proc_current = null;
	proc_schedule();

	proc_release(flags);
}

void proc_yield(void) {
	u32 flags = proc_lock();

	proc_current->time_end = pit_systime_msec;

	proc_release(flags);
}

void proc_schedule(void) {
	if (!ready_q || ready_q == proc_current)
		return;

	if (proc_current) {
		if (pit_systime_msec < proc_current->time_end)
			return;

		proc_t** it = &ready_q;
		while (*it)
			it = &(*it)->next;
		*it = proc_current;
	}

	proc_t* next_proc = ready_q;

	if (ready_q->next)
		ready_q = ready_q->next;

	next_proc->next = null;

	proc_switch(next_proc);
}

u32 proc_lock(void) {
	u32 flags = getf();
	cli();
	spinlock_lock(&sched_lock);
	return flags;
}

void proc_release(u32 flags) {
	spinlock_release(&sched_lock);
	if (flags & EFLAGS_IF)
		sti();
}

