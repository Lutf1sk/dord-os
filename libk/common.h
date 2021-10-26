#ifndef COMMON_H
#define COMMON_H

#include <stdint.h>
#include <stddef.h>

typedef uint64_t u64;
typedef uint32_t u32;
typedef uint16_t u16;
typedef uint8_t u8;

typedef int64_t i64;
typedef int32_t i32;
typedef int16_t i16;
typedef int8_t i8;

typedef size_t usz;

#define INLINE inline __attribute__((always_inline))
#define NORETURN __attribute__((noreturn))
#define FLATTEN __attribute__((flatten))
#define PACKED __attribute__((packed))

#define KB(x) (x * 1024L)
#define MB(x) (KB(x) * 1024L)

#endif
