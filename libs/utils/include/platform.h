#pragma once

#include "types.h"

#ifdef _MSC_VER
#include <intrin.h>
#define _CRT_SECURE_NO_WARNINGS
#endif

#if defined(_MSC_VER)
#define TODO(msg) __pragma(message("TODO: " msg))
#elif defined(__GNUC__) || defined(__clang__)
#define DO_PRAGMA(x) _Pragma(#x)
#define TODO(msg) DO_PRAGMA(message("TODO: " msg))
#else
#define TODO(msg)
#endif

#if defined(_MSC_VER)
#define UNREACHABLE() __assume(0)
#elif defined(__GNUC__) || defined(__clang__)
#define UNREACHABLE() __builtin_unreachable()
#else
#define UNREACHABLE()
#endif

#define UNIMPLEMENTED(msg)           \
    do {                             \
        TODO("UNIMPLEMENTED: " msg); \
        UNREACHABLE();               \
    } while (0)

static inline u8 bitscan_forward(u64 bb) {
#ifdef _MSC_VER
    unsigned long idx;
    _BitScanForward64(&idx, bb);
    return (u8)idx;
#else
    return (u8)__builtin_ctzll(bb);
#endif
}

static inline u8 popcnt64(u64 bb) {
#ifdef _MSC_VER
    return (u8)__popcnt64(bb);
#else
    return (u8)__builtin_popcountll(bb);
#endif
}
