#include <fs.h>
#include <proc.h>
#include <debug_io.h>

#include <err.h>
#include <syscall.h>

typedef
struct regs {
	usz a;
	usz c;
	usz d;
	usz b;
} regs_t;

err_t sys_handler(regs_t* regs) {
	switch (regs->a) {
	case SYS_PRINT:
		dbg_write(null, (void*)regs->c, regs->d);
		return OK;

	case SYS_YIELD:
		proc_yield();
		return OK;

	case SYS_FREAD:
		return fread((char*)regs->c, (void**)regs->d, (usz*)regs->b);

	default:
		return ERR_UNKNTYPE;
	}
}
