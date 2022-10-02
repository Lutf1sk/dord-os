#ifndef MEMORY_H
#define MEMORY_H

#include <common.h>

u32 strlen(const char* str);
u8 strneq(const char* str1, const char* str2, u32 maxlen);

static INLINE
void mcpy8(void* dst, const void* src, usz count) {
	asm volatile ("cld\n\trep movsb" : "=D"(dst), "=S"(src), "=c"(count) : "D"(dst), "S"(src), "c"(count) : "memory", "cc");
}

static INLINE
void mcpy16(void* dst, const void* src, usz count) {
	asm volatile ("cld\n\trep movsw" : "=D"(dst), "=S"(src), "=c"(count) : "D"(dst), "S"(src), "c"(count) : "memory", "cc");
}

static INLINE
void mcpy32(void* dst, const void* src, usz count) {
	asm volatile ("cld\n\trep movsd" : "=D"(dst), "=S"(src), "=c"(count) : "D"(dst), "S"(src), "c"(count) : "memory", "cc");
}

void* mmove8(void* dst, const void* src, u32 count);
void* mmove16(void* dst, const void* src, u32 count);
void* mmove32(void* dst, const void* src, u32 count);

u8 meq8(const void* p1, const void* p2, u32 count);

static INLINE
void mset8(void* dst, u8 val, u32 count) {
	asm volatile ("rep stosb" : "=D"(dst), "=c"(count) : "D"(dst), "c"(count), "a"(val) : "memory", "cc");
}

static INLINE
void mset16(void* dst, u16 val, u32 count) {
	asm volatile ("rep stosw" : "=D"(dst), "=c"(count) : "D"(dst), "c"(count), "a"(val) : "memory", "cc");
}

static INLINE
void mset32(void* dst, u32 val, u32 count) {
	asm volatile ("rep stosd" : "=D"(dst), "=c"(count) : "D"(dst), "c"(count), "a"(val) : "memory", "cc");
}

#endif
