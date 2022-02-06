#ifndef PROC_H
#define PROC_H 1

#include <common.h>

#define PROC_RUN	1
#define PROC_WAIT	2
#define PROC_SUSP	3

typedef
struct proc {
	void* sp;
	void* entry;
	struct proc* next;
	u32 time_end;
	char* name;
} proc_t;

extern proc_t* proc_current;

proc_t* proc_create(void* entry, char* name);
void proc_register(proc_t* proc);
void proc_schedule(void);

void proc_yield(void);
void proc_exit(void);

// proc.asm
void proc_init(proc_t*);
void proc_switch(proc_t*);

NOREORDER
volatile u32 proc_lock(void);

NOREORDER
void proc_release(volatile u32 flags);

#endif
