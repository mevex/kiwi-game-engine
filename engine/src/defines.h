#pragma once

/******************************************************************
This file is an ever growing collection of type definitions,
macros and templates that can be usefull in every part of the code
******************************************************************/

/*
        COMPILER DETECTION
*/
// NOTE: _MSC_VER GENERALLY means we're building with MSVC,
// but it can be also defined by the intel compiler. Since we
// don't plan on supporting it, the check against it is missing.
#ifdef _MSC_VER
#define KIWI_MSVC
#else
#error "Only MSVC compiler is supported for now! OMEGALUL"
#endif

/*
        OS DETECTION
*/
#if defined(WIN32) || defined(_WIN32) || defined(__WIN32__)
#ifndef _WIN64
#error "64-bit Windows required!"
#define KIWI_WIN
#endif
#else
#error "Only windows is supported for now! OMEGALUL"
#endif

/*
        EXORT MACROS
*/
#ifdef KIWI_MSVC
#ifdef KIWI_ENGINE_EXPORTS
#define KIWI_API __declspec(dllexport)
#else
#define KIWI_API __declspec(dllimport)
#endif
#endif

/*
        INLINE MACROS
*/
#ifdef KIWI_MSVC
#define KIWI_FORCEINLINE __forceinline
#define KIWI_FORBIDINLINE __declspec(noinline)
#else
// TODO: This should work both on gcc and clang, but i haven't done too much
// research about it. Also I don't know a way of preventing inlining on those compilers
#define KIWI_FORCEINLINE inline __attribute__((always_inline))
#define KIWI_FORBIDINLINE
#endif

/*
        RUNTIME AND STATIC ASSERTION
*/
// TODO: allow for more complicated behaviours
#if defined(KIWI_MSVC) && defined(KIWI_SLOW)
#define DebugBreak() __debugbreak()
#define Assert(Expression) \
        if (!(Expression)) \
        DebugBreak()
#define AssertMsg(Expression, Message) \
        if (!(Expression))             \
        DebugBreak()
#define StaticAssert(Expression) static_assert(Expression)
#define StaticAssertMsg(Expression, Message) static_assert(Expression, Message)
#else
#define DebugBreak()
#define Assert(Expression)
#define AssertMsg(Expression, Message)
#define StaticAssert(Expression)
#define StaticAssertMsg(Expression, Message)
#endif

// TRUE and FALSE definitions that I use for macro
#define TRUE 1
#define FALSE 0

/*
        CUSTOM TYPES
*/
typedef unsigned char i8;
typedef unsigned short i16;
typedef unsigned int i32;
typedef unsigned long long i64;

typedef char u8;
typedef short u16;
typedef int u32;
typedef long long u64;

typedef float f32;
typedef double f64;

typedef u8 byte;

typedef u8 b8;
typedef u32 b32;

/*
        CUSTOM TYPES SIZE ASSERTIONS
*/
StaticAssertMsg(sizeof(i8) == 1, "i8 has an unexpected size");
StaticAssertMsg(sizeof(i16) == 2, "i16 has an unexpected size");
StaticAssertMsg(sizeof(i32) == 4, "i32 has an unexpected size");
StaticAssertMsg(sizeof(i64) == 8, "i64 has an unexpected size");

StaticAssertMsg(sizeof(u8) == 1, "i8 has an unexpected size");
StaticAssertMsg(sizeof(u16) == 2, "i16 has an unexpected size");
StaticAssertMsg(sizeof(u32) == 4, "i32 has an unexpected size");
StaticAssertMsg(sizeof(u64) == 8, "i64 has an unexpected size");

StaticAssertMsg(sizeof(f32) == 4, "f32 has an unexpected size");
StaticAssertMsg(sizeof(f64) == 8, "f64 has an unexpected size");

StaticAssertMsg(sizeof(void *) == 8, "32-bit compilation not supported");

/*
        MISC
*/
#define local_persist static
#define global_var static
#define internal_func static

#define KiB(value) ((value) * 1024LL)
#define MiB(value) (KiB(value) * 1024LL)
#define GiB(value) (MiB(value) * 1024LL)
#define TiB(value) (GiB(value) * 1024LL)

#define ArrayCount(array) (sizeof(array) / sizeof((array)[0]))
#define CheckFlags(Flags, FlagsToCheck) ((Flags) & (FlagsToCheck))
#define FourCC(String) (u32)((String[0] << 0) | (String[1] << 8) | (String[2] << 16) | (String[3] << 24))

template <typename T>
inline T Max(T A, T B) { return A > B ? A : B; }
template <typename T>
inline T Min(T A, T B) { return A < B ? A : B; }
template <typename T>
inline T Clamp(T Value, T Min, T Max) { return Max(Min(Value, Max), Min); }