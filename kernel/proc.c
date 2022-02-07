#include <proc.h>
#include <pmman.h>
#include <debug_io.h>
#include <spinlock.h>
#include <asm.h>

#include <x86/cpuid.h>

#include <drivers/pit.h>

proc_t* proc_current = null;
proc_t* proc_idle = null;

static proc_t* ready_q = null;
static proc_t* blocked_q = null;
static proc_t* waiting_q = null;

slock_t sched_lock = SLOCK_INIT();

#define PROC_SIZE KB(4)

void panic(char*);

void proc_idle_fn(void) {
	while (1)
		hlt();
}

void proc_init_scheduler(void) {
	proc_idle = proc_create(proc_idle_fn, "idle");
	proc_idle->state = PROC_IDLE;
}

proc_t* proc_create(void* entry, char* name) {
	u8* new = pmman_alloc(&pmman_kernel_map, PROC_SIZE);
	proc_t* proc = (proc_t*)new;
	proc->entry = entry;
	proc->next = null;
	proc->name = name;

	u32* stack = (u32*)(new + PROC_SIZE);

	proc->sp = stack;
	proc_init(proc);
	return proc;
}

void proc_sleep_msec(u32 msec) {
	volatile u32 flags = proc_lock();

	proc_current->time_end = pit_systime_msec + msec;

	proc_current->next = waiting_q;
	waiting_q = proc_current;
	proc_current->state = PROC_SLEEP;

// 	dbg_printf("%s sleeping for %udms\n", proc_current->name, msec);
	proc_schedule();

	proc_release(flags);
}

void proc_register(proc_t* proc) {
	volatile u32 flags = proc_lock();

	proc->state = PROC_READY;
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
	// Add any processes that are done waiting to the front of the ready queue
	{
		proc_t** it = &waiting_q;
		while (*it) {
			if (pit_systime_msec >= (*it)->time_end) {
				proc_t* proc = (*it);

// 				dbg_printf("%s awoke\n", proc->name);

				(*it) = proc->next;

				proc->state = PROC_READY;
				proc->next = ready_q;
				ready_q = proc;
				continue;
			}
			it = &(*it)->next;
		}
	}

	// Switch to idle process if all available processes are blocked
	if (!ready_q) {
		if ((!proc_current || (proc_current->state != PROC_ACTIVE && proc_current->state != PROC_IDLE)) && proc_idle) {
// 			dbg_printf("[CPU%ud] Going idle...\n", cpu_lapic_id());
			proc_switch(proc_idle);
		}
		return;
	}

	// Add current process to the end of the ready queue
	if (proc_current && proc_current->state == PROC_ACTIVE) {
		if (pit_systime_msec < proc_current->time_end)
			return;

		proc_t** it = &ready_q;
		while (*it)
			it = &(*it)->next;
		*it = proc_current;
		proc_current->next = null;
	}

	proc_t* next_proc = ready_q;
	ready_q = ready_q->next;
	next_proc->next = null;

	if (proc_current == next_proc)
		return;

// 	dbg_printf("[CPU%ud] Switching to %s\n", cpu_lapic_id(), next_proc->name);

	next_proc->state = PROC_ACTIVE;
	proc_switch(next_proc);
}

u32 proc_lock(void) {
	volatile u32 flags = getf();
	cli();
	spinlock_lock(&sched_lock);
	return flags;
}

void proc_release(volatile u32 flags) {
	spinlock_release(&sched_lock);
	setf(flags);
}

