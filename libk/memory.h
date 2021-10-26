#ifndef MEMORY_H
#define MEMORY_H

#include "common.h"

u32 strlen(const char* str);
u8 strneq(const char* str1, const char* str2, u32 maxlen);

void* mcpy32(void* dst, const void* src, u32 count);
void* mcpy16(void* dst, const void* src, u32 count);
void* mcpy8(void* dst, const void* src, u32 count);

void* mmove16(void* dst, const void* src, u32 count);

u8 meq8(const void* p1, const void* p2, u32 count);

void* mset8(void* dst, u8 val, u32 count);
void* mset16(void* dst, u16 val, u32 count);
void* mset32(void* dst, u32 val, u32 count);

#endif
