
#pragma once

#include <stddef.h>
#include <stdint.h>

typedef unsigned int uint;
typedef unsigned short ushort;
typedef unsigned char uchar;
typedef unsigned long long ulong;

typedef uint8_t u8;
typedef int8_t i8;
typedef uint16_t u16;
typedef int16_t i16;
typedef uint32_t u32;
typedef int32_t i32;
typedef uint64_t u64;
typedef int64_t i64;
typedef float f32;
typedef double f64;

typedef  int b32;
typedef _Bool b8;

typedef struct range64 {
    u64 offset;
    u64 size;
} range64;

typedef struct range32 {
    u32 offset;
    u32 size;
} range32;

#if defined (__clang__) || defined (__GNUC__)
#define STATIC_ASSERT _Static_assert
#else
#define STATIC_ASSERT static_assert
#endif

STATIC_ASSERT(sizeof(u8) == 1, "size of u8: 1 byte");
STATIC_ASSERT(sizeof(u16) == 2, "size of u16: 2 bytes");
STATIC_ASSERT(sizeof(u32) == 4, "size of u32: 4 byte");
STATIC_ASSERT(sizeof(u64) == 8, "size of u64: 8 byte");
STATIC_ASSERT(sizeof(i8) == 1, "size of i8: 1 byte");
STATIC_ASSERT(sizeof(i16) == 2, "size of i16: 2 bytes");
STATIC_ASSERT(sizeof(i32) == 4, "size of i32: 4 bytes");
STATIC_ASSERT(sizeof(i64) == 8, "size of i64: 8 bytes");
STATIC_ASSERT(sizeof(f32) == 4, "size of f32: 4 bytes");
STATIC_ASSERT(sizeof(f64) == 8, "size of f64: 8 bytes");

#define benar 1
#define salah 0


#define true 1
#define false 0

#if defined(_WIN32) || defined(__CYGWIN__)
    /* Windows: Export if EITHER library is being built */
    #if defined(COMMON_BUILDING) || defined(CORE_BUILDING)
        #define DHEMITUS_API __declspec(dllexport)
    #else
        #define DHEMITUS_API __declspec(dllimport)
    #endif
#else
    /* macOS/Linux: Use visibility attributes */
    #if __GNUC__ >= 4
        #define DHEMITUS_API __attribute__((visibility("default")))
        #define DHEMITUS_HIDDEN __attribute__((visibility("hidden")))
    #else
        #define DHEMITUS_API
        #define DHEMITUS_HIDDEN
    #endif
#endif

#define CLAMP(value, min, max) ((value <= min) ? min : (value >= max) ? max : value)

#define MIN(x, y) (x < y ? x : y)
#define MAX(x, y) (x > y ? x : y)
