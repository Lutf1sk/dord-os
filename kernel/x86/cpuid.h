#ifndef CPUID_H
#define CPUID_H

#include <common.h>

typedef
struct PACKED CPUInfo {
	// ECX
	u8 sse3		: 1;
	u8 pclmul	: 1;
	u8 dtes64	: 1;
	u8 monitor	: 1;
	u8 ds_cpl	: 1;
	u8 vmx		: 1;
	u8 smx		: 1;
	u8 est		: 1;
	u8 tm2		: 1;
	u8 ssse3	: 1;
	u8 cid		: 1;
	u8 fma		: 1;
	u8 cx16		: 1;
	u8 etprd	: 1;
	u8 pdcm		: 1;
	u8 : 1; // RESERVED
	u8 pcid		: 1;
	u8 dca		: 1;
	u8 sse4_1	: 1;
	u8 sse4_2	: 1;
	u8 x2apic	: 1;
	u8 movbe	: 1;
	u8 popcnt	: 1;
	u8 aes		: 1;
	u8 xsave	: 1;
	u8 osxsave	: 1;
	u8 avx		: 1;

	// EDX
	u8 fpu		: 1;
	u8 vme		: 1;
	u8 de		: 1;
	u8 pse		: 1;
	u8 tsc		: 1;
	u8 msr		: 1;
	u8 pae		: 1;
	u8 mce		: 1;
	u8 cx8		: 1;
	u8 apic		: 1;
	u8 : 1; // RESERVED
	u8 sep		: 1;
	u8 mtrr		: 1;
	u8 pge		: 1;
	u8 mca		: 1;
	u8 cmov		: 1;
	u8 pat		: 1;
	u8 pse36	: 1;
	u8 psn		: 1;
	u8 clfsh	: 1;
	u8 : 1; // RESERVED
	u8 ds		: 1;
	u8 acpi		: 1;
	u8 mmx		: 1;
	u8 fxsr		: 1;
	u8 sse		: 1;
	u8 sse2		: 1;
	u8 ss		: 1;
	u8 htt		: 1;
	u8 tm1		: 1;
	u8 ia64		: 1;
	u8 pbe		: 1;
} CPUInfo;

typedef
struct PACKED CPUExtendedInfo {
	// ECX
	u8 lahf_lm		: 1;
	u8 cmp_legacy	: 1;
	u8 svm			: 1;
	u8 extapic		: 1;
	u8 cr8_legacy	: 1;
	u8 abm			: 1;
	u8 sse4a		: 1;
	u8 misalignsse	: 1;
	u8 prefetch3dnow: 1;
	u8 osvw			: 1;
	u8 ibs			: 1;
	u8 xop			: 1;
	u8 skinit		: 1;
	u8 wdt			: 1;
	u8 : 1; // RESERVED
	u8 lwp			: 1;
	u8 fma4			: 1;
	u8 tce			: 1;
	u8 : 1; // RESERVED
	u8 nodeid_msr	: 1;
	u8 : 1; // RESERVED
	u8 tbm			: 1;
	u8 topoext		: 1;
	u8 perfctr_core	: 1;
	u8 perfctr_nb	: 1;
	u8 : 1; // RESERVED
	u8 dbx			: 1;
	u8 perftsc		: 1;
	u8 pcx_l2i		: 1;
	u8 : 1; // RESERVED
	u8 : 1; // RESERVED
	u8 : 1; // RESERVED

	// EDX
	u8 fpu		: 1;
	u8 vme		: 1;
	u8 de		: 1;
	u8 pse		: 1;
	u8 tsc		: 1;
	u8 msr		: 1;
	u8 pae		: 1;
	u8 mce		: 1;
	u8 cx8		: 1;
	u8 apic		: 1;
	u8 : 1; // RESERVED
	u8 syscall	: 1;
	u8 mtrr		: 1;
	u8 pge		: 1;
	u8 mca		: 1;
	u8 cmov		: 1;
	u8 pat		: 1;
	u8 pse36	: 1;
	u8 : 1; // RESERVED
	u8 mp		: 1;
	u8 nx		: 1;
	u8 : 1; // RESERVED
	u8 mmxext	: 1;
	u8 mmx		: 1;
	u8 fxsr		: 1;
	u8 fxsr_opt	: 1;
	u8 pdpe1gb	: 1;
	u8 rdtscp	: 1;
	u8 : 1; // RESERVED
	u8 lm		: 1;
	u8 ext3dnow	: 1;
	u8 s_3dnow	: 1;
} CPUExtendedInfo;

static
const char* cpu_vendor_str() {
	static char str[13];

	__asm__ volatile ("mov ebx, 0	\n"
				  "push %0		\n"
				  "mov eax, 0	\n"
				  "cpuid		\n"
				  "pop %0		\n"
				  "mov [%0 + 0], ebx	\n"
				  "mov [%0 + 4], edx	\n"
				  "mov [%0 + 8], ecx	\n"
				  : : "a"(str)
				  : "ebx", "ecx", "edx");

	str[12] = 0;

	return str;
}

static inline INLINE
u32 cpu_highest_ext_func() {
	u32 highest_function = 0;
	__asm__ volatile ("mov ebx, 0			\n"
				  "mov eax, 0x80000000	\n"
				  "cpuid				\n"
				  : "=a"(highest_function)
				  : : "ebx");

	return highest_function;
}

static
const char* cpu_brand_str() {
	static char str[48];

	__asm__ volatile ("mov edi, %0			\n"
				  "mov ebx, 0			\n"
				  "mov eax, 0x80000002	\n"
				  "cpuid				\n"
				  "mov [edi + 0], eax		\n"
				  "mov [edi + 4], ebx		\n"
				  "mov [edi + 8], ecx		\n"
				  "mov [edi + 12], edx		\n"

				  "mov ebx, 0			\n"
				  "mov eax, 0x80000003	\n"
				  "cpuid				\n"
				  "mov [edi + 16], eax		\n"
				  "mov [edi + 20], ebx		\n"
				  "mov [edi + 24], ecx		\n"
				  "mov [edi + 28], edx		\n"

				  "mov ebx, 0			\n"
				  "mov eax, 0x80000004	\n"
				  "cpuid				\n"
				  "mov [edi + 32], eax		\n"
				  "mov [edi + 36], ebx		\n"
				  "mov [edi + 40], ecx		\n"
				  "mov [edi + 44], edx		\n"

				  : : "a"(str)
				  : "ebx", "ecx", "edx", "edi");

	return str;
}

static inline INLINE
CPUInfo cpu_info() {
	CPUInfo info;

	asm volatile ("mov ebx, 0	\n"
				  "push %0		\n"
				  "mov eax, 1	\n"
				  "cpuid		\n"
				  "pop %0		\n"
				  "mov [%0 + 0], ecx	\n"
				  "mov [%0 + 4], edx	\n"
				  : : "a"(&info)
				  : "ebx", "ecx", "edx");

	return info;
}

static inline INLINE
CPUExtendedInfo cpu_extended_info() {
	CPUExtendedInfo info;

	asm volatile ("mov ebx, 0			\n"
				  "push %0				\n"
				  "mov eax, 0x80000001	\n"
				  "cpuid				\n"
				  "pop %0				\n"
				  "mov [%0 + 0], ecx	\n"
				  "mov [%0 + 4], edx	\n"
				  : : "a"(&info)
				  : "ebx", "ecx", "edx");

	return info;

}

#endif
