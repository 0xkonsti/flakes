#pragma once

#include <immintrin.h>
#include "types.h"

extern u64 knight_move_lut[64];

extern u64 pawn_attack_lut[2][64];
extern u64 pawn_single_lut[2][64];
extern u64 pawn_double_lut[2][64];

extern u64 king_move_lut[64];

extern u64 bishop_mask[64];
extern u64 rook_mask[64];
extern u64* bishop_table[64];
extern u64* rook_table[64];

void move_lut_init(void);

static inline u64 bishop_attacks(u8 sq, u64 occ) {
    u64 m = bishop_mask[sq];
    return bishop_table[sq][_pext_u64(occ & m, m)];
}

static inline u64 rook_attacks(u8 sq, u64 occ) {
    u64 m = rook_mask[sq];
    return rook_table[sq][_pext_u64(occ & m, m)];
}

static inline u64 queen_attacks(u8 sq, u64 occ) {
    return bishop_attacks(sq, occ) | rook_attacks(sq, occ);
}
