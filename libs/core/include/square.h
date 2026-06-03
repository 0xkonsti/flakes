#pragma once

#include <ctype.h>
#include "types.h"

#define SQ_A1 0ULL
#define SQ_B1 1ULL
#define SQ_C1 2ULL
#define SQ_D1 3ULL
#define SQ_E1 4ULL
#define SQ_F1 5ULL
#define SQ_G1 6ULL
#define SQ_H1 7ULL
#define SQ_A2 8ULL
#define SQ_B2 9ULL
#define SQ_C2 10ULL
#define SQ_D2 11ULL
#define SQ_E2 12ULL
#define SQ_F2 13ULL
#define SQ_G2 14ULL
#define SQ_H2 15ULL
#define SQ_A3 16ULL
#define SQ_B3 17ULL
#define SQ_C3 18ULL
#define SQ_D3 19ULL
#define SQ_E3 20ULL
#define SQ_F3 21ULL
#define SQ_G3 22ULL
#define SQ_H3 23ULL
#define SQ_A4 24ULL
#define SQ_B4 25ULL
#define SQ_C4 26ULL
#define SQ_D4 27ULL
#define SQ_E4 28ULL
#define SQ_F4 29ULL
#define SQ_G4 30ULL
#define SQ_H4 31ULL
#define SQ_A5 32ULL
#define SQ_B5 33ULL
#define SQ_C5 34ULL
#define SQ_D5 35ULL
#define SQ_E5 36ULL
#define SQ_F5 37ULL
#define SQ_G5 38ULL
#define SQ_H5 39ULL
#define SQ_A6 40ULL
#define SQ_B6 41ULL
#define SQ_C6 42ULL
#define SQ_D6 43ULL
#define SQ_E6 44ULL
#define SQ_F6 45ULL
#define SQ_G6 46ULL
#define SQ_H6 47ULL
#define SQ_A7 48ULL
#define SQ_B7 49ULL
#define SQ_C7 50ULL
#define SQ_D7 51ULL
#define SQ_E7 52ULL
#define SQ_F7 53ULL
#define SQ_G7 54ULL
#define SQ_H7 55ULL
#define SQ_A8 56ULL
#define SQ_B8 57ULL
#define SQ_C8 58ULL
#define SQ_D8 59ULL
#define SQ_E8 60ULL
#define SQ_F8 61ULL
#define SQ_G8 62ULL
#define SQ_H8 63ULL

#define SQUARE(file, rank) (u8)((rank) * 8 + (file))

#define ALGEBRAIC_INVALID 0xFF

static inline u8 square_from_algebraic(char const* algebraic) {
    if (!algebraic || algebraic[0] == '\0' || algebraic[1] == '\0' || algebraic[2] != '\0') {
        return ALGEBRAIC_INVALID;
    }

    char file = (char)toupper(algebraic[0]);
    char rank = algebraic[1];

    if (file < 'A' || file > 'H' || rank < '1' || rank > '8') {
        return ALGEBRAIC_INVALID;
    }

    return SQUARE(file - 'A', rank - '1');
}
