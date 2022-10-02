#include <memory.h>

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

void* mmove8(void* dst, const void* src, u32 count) {
	u8* d_it = dst;
	const u8* s_it = src;
	while (count--)
		*(d_it++) = *(s_it++);
	return dst;
}

void* mmove16(void* dst, const void* src, u32 count) {
	u16* d_it = dst;
	const u16* s_it = src;
	while (count--)
		*(d_it++) = *(s_it++);
	return dst;
}

void* mmove32(void* dst, const void* src, u32 count) {
	u32* d_it = dst;
	const u32* s_it = src;
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

