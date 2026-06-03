#pragma once

#include "board.h"

extern u64 zobrist_pieces[7][2][64];
extern u64 zobrist_side;
extern u64 zobrist_castling[4];
extern u64 zobrist_ep[8];

void zobrist_init(void);
u64 zobrist_hash(chessboard_t const* b);

// TODO:
// After a move:
// st->hash ^= zobrist_piece_key(piece, color, from);
// st->hash ^= zobrist_piece_key(piece, color, to);
// if (capture)   st->hash ^= zobrist_piece_key(cap_piece, cap_color, to);
// if (promo) {
//     st->hash ^= zobrist_piece_key(PAWN, color, to);
//     st->hash ^= zobrist_piece_key(promo_piece, color, to);
// }
// st->hash ^= zobrist_side_key();
// st->hash ^= zobrist_castling_key(old_rights);
// st->hash ^= zobrist_castling_key(new_rights);  // same effect as XOR diff
// st->hash ^= zobrist_ep_key(old_ep);
// st->hash ^= zobrist_ep_key(new_ep);
// To unmake just restore the old st->hash

static inline u64 zobrist_piece_key(piece_t p, color_t c, u8 sq) {
    return zobrist_pieces[p][c][sq];
}

static inline u64 zobrist_side_key(void) {
    return zobrist_side;
}

static inline u64 zobrist_castling_key(u8 rights) {
    u64 key = 0;
    if (rights & 1) key ^= zobrist_castling[0];
    if (rights & 2) key ^= zobrist_castling[1];
    if (rights & 4) key ^= zobrist_castling[2];
    if (rights & 8) key ^= zobrist_castling[3];
    return key;
}

static inline u64 zobrist_ep_key(u8 sq) {
    return sq == EN_PASSANT_NONE ? 0 : zobrist_ep[sq & 7];
}
