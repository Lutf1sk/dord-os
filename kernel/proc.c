#include <proc.h>
#include <pmman.h>
#include <debug_io.h>
#include <spinlock.h>
#include <asm.h>

#include <drivers/pit.h>

proc_t* proc_current = null;

slock_t sched_lock = SLOCK_INIT();

void panic(char*);

proc_t* proc_create(void* entry) {
	u8* new = pmman_alloc(&pmman_kernel_map, KB(4));
	proc_t* proc = (proc_t*)new;
	proc->entry = entry;
	proc->next = proc;

	u32* stack = (u32*)(new + KB(4));

	proc->sp = stack;
	proc_init(proc);
	return proc;
}

void proc_register(proc_t* proc) {
	u32 flags = proc_lock();

	if (!proc_current)
		panic("proc_register called while proc_current==null");

	proc->next = proc_current->next;
	if (!proc->next)
		proc->next = proc_current;
	proc_current->next = proc;

	proc_release(flags);
}

void proc_end(void) {
	panic("proc_end\n");
}

void proc_schedule(void) {
	if (!proc_current || !proc_current->next) {
		return; // There are no (other) processes
	}
	if (pit_systime_msec < proc_current->time_end)
		return; // The process has time left

	proc_switch(proc_current->next);
}

void proc_yield(void) {
	u32 flags = proc_lock();

	proc_current->time_end = pit_systime_msec;

	proc_release(flags);
}

u32 proc_lock(void) {
	u32 flags = getf();
	cli();
	spinlock_lock(&sched_lock);
// 	dbg_printf("Lock aquired with flags=%hz\n", flags);
	return flags;
}

void proc_release(u32 flags) {
	spinlock_release(&sched_lock);
// 	dbg_printf("Lock released with flags=%hz\n", flags);
	if (flags & EFLAGS_IF)
		sti();
}

