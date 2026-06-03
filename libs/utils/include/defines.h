#pragma once

#define MIN(a, b) ((a) < (b) ? (a) : (b))
#define MAX(a, b) ((a) > (b) ? (a) : (b))
#define CLAMP(x, lo, hi) (MIN(MAX((x), (lo)), (hi)))

#define ARRAY_LEN(x) (sizeof(x) / sizeof((x)[0]))

#define ALIGN_UP(x, a) (((x) + (a) - 1) & ~((a) - 1))
#define ALIGN_DOWN(x, a) ((x) & ~((a) - 1))

#define BIT(n) (1ULL << (n))
#define BIT_GET(x, n) (((x) >> (n)) & 1ULL)
#define BIT_SET(x, n) ((x) |= BIT(n))
#define BIT_CLEAR(x, n) ((x) &= ~BIT(n))

#define UNUSED(x) ((void)(x))

#define STRINGIFY(x) #x
#define CONCAT(a, b) a##b
