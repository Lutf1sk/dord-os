#ifndef PROC_H
#define PROC_H 1

#include <common.h>

#define PROC_ACTIVE 0
#define PROC_READY	1
#define PROC_SLEEP	2
#define PROC_IDLE	3

typedef
struct proc {
	void* sp;
	void* entry;
	struct proc* next;
	u32 time_end;
	char* name;
	u32 state;
} proc_t;

extern proc_t* proc_current;

void proc_init_scheduler(void);

proc_t* proc_create(void* entry, char* name);
void proc_register(proc_t* proc);
void proc_schedule(void);

void proc_sleep_msec(u32 msec);
void proc_yield(void);
void proc_exit(void);

// proc.asm
void proc_init(proc_t*);

NOREORDER
void proc_switch(proc_t*);

NOREORDER
volatile u32 proc_lock(void);

NOREORDER
void proc_release(volatile u32 flags);

#endif
