#pragma once

#include "types.h"

#define MOVE_NONE 0

#define MOV_FROM(m) ((m) & 0x3f)
#define MOV_TO(m) (((m) >> 6) & 0x3f)
#define MOV_FLAGS(m) (((m) >> 12) & 0x3)
#define MOV_PROMO(m) (((m) >> 14) & 0x3)
#define MOV_MOVING(m) (((m) >> 16) & 0x7)
#define MOV_CAPTURED(m) (((m) >> 19) & 0x7)

// Move encoding: 32-bit
// bits 0-5:    from square (0-63)
// bits 6-11:   to square   (0-63)
// bits 12-13:  flags       (0=normal, 1=promotion, 2=enpassant, 3=castling)
// bits 14-15:  promo piece (only when flags==promotion)
// bits 16-18:  moving piece (piece_t, 1=PAWN..6=KING)
// bits 19-21:  captured piece (piece_t, 0=none, 1=PAWN..6=KING)
// bits 22-31:  unused
typedef u32 move_t;

enum { MF_NORMAL, MF_PROMOTION, MF_ENPASSANT, MF_CASTLING };

static inline move_t move_create(u8 from, u8 to, u8 flags, u8 promo, u8 moving, u8 captured) {
    return (move_t)(from | (to << 6) | (flags << 12) | (promo << 14) | (moving << 16) | (captured << 19));
}
