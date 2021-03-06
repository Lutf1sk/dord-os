#ifndef EXCEPTIONS_H
#define EXCEPTIONS_H

#define EXCEPT_DIV_BY_0 0
#define EXCEPT_DEBUG 1
#define EXCEPT_NON_MASKABLE_INTERRUPT 2
#define EXCEPT_BREAKPOINT 3
#define EXCEPT_OVERFLOW 4
#define EXCEPT_BOUND_RANGE_EXCEEDED 5
#define EXCEPT_INVAL_OP 6
#define EXCEPT_DEV_NOT_AVAIL 7
#define EXCEPT_DOUBLE_FAULT 8
#define EXCEPT_COPROC_SEGMENT_OVERRUN 9
#define EXCEPT_INVAL_TSS 10
#define EXCEPT_SEG_NOT_PRESENT 11
#define EXCEPT_STACK_SEG_FAULT 12
#define EXCEPT_GENERAL_PROTECTION_FAULT 13
#define EXCEPT_PAGE_FAULT 14
#define EXCEPT_FPU_ERROR 16
#define EXCEPT_ALIGN_CHECK 17
#define EXCEPT_MACHINE_CHECK 18
#define EXCEPT_SIMD_FPU_ERROR 19
#define EXCEPT_VIRT_EXCEPT 20
#define EXCEPT_SECURITY_EXCEPT 30

#endif
