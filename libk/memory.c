#include "memory.h"

u32 strlen(const char* str) {
	const char* it = str;
	while (*(it++))
		;
	return it - str - 1;
}

u8 strneq(const char* str1, const char* str2, u32 maxlen) {
	u32 i = 0;
	while (i < maxlen) {
		if (str1[i] != str2[i])
			return 0;
		if (!str1[i])
			return 1;
		++i;
	}
	return 1;
}

void* mcpy32(void* dst, const void* src, u32 count) {
	__asm__ volatile ("rep movsd" : : "D"(dst), "S"(src), "c"(count) : "memory", "cc");
	return dst;
}

void* mcpy16(void* dst, const void* src, u32 count) {
	__asm__ volatile ("rep movsw" : : "D"(dst), "S"(src), "c"(count) : "memory", "cc");
	return dst;
}

void* mcpy8(void* dst, const void* src, u32 count) {
	__asm__ volatile ("rep movsb" : : "D"(dst), "S"(src), "c"(count) : "memory", "cc");
	return dst;
}

void* mmove16(void* dst, const void* src, u32 count) {
	u16* d_it = dst;
	const u16* s_it = src;
	while (count--)
		*(d_it++) = *(s_it++);
	return dst;
}

u8 meq8(const void* p1, const void* p2, u32 bytes) {
	const u8* it1 = p1;
	const u8* it2 = p2;

	while (bytes--)
		if (*(it1++) != *(it2++))
			return 0;
	return 1;
}

void* mset8(void* dst, u8 val, u32 count) {
	__asm__ volatile ("rep stosb" : : "D"(dst), "c"(count), "a"(val) : "memory", "cc");
	return dst;
}

void* mset16(void* dst, u16 val, u32 count) {
	__asm__ volatile ("rep stosw" : : "D"(dst), "c"(count), "a"(val) : "memory", "cc");
	return dst;
}

void* mset32(void* dst, u32 val, u32 count) {
	__asm__ volatile ("rep stosd" : : "D"(dst), "c"(count), "a"(val) : "memory", "cc");
	return dst;
}

