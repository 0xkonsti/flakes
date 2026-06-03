#include "zobrist.h"

u64 zobrist_pieces[7][2][64];
u64 zobrist_side;
u64 zobrist_castling[4];
u64 zobrist_ep[8];

static u64 splitmix64(u64* s) {
    u64 z = (*s += 0x9e3779b97f4a7c15ULL);
    z = (z ^ (z >> 30)) * 0xbf58476d1ce4e5b9ULL;
    z = (z ^ (z >> 27)) * 0x94d049bb133111ebULL;
    return z ^ (z >> 31);
}

void zobrist_init(void) {
    static bool initialized = false;
    if (initialized) return;
    initialized = true;

    u64 seed = 0xabcd3059e8f42b17ULL;

    for (piece_t p = NO_PIECE; p <= KING; p++)
        for (color_t c = WHITE; c <= BLACK; c++)
            for (u8 sq = 0; sq < 64; sq++) zobrist_pieces[p][c][sq] = splitmix64(&seed);

    zobrist_side = splitmix64(&seed);

    for (u8 i = 0; i < 4; i++) zobrist_castling[i] = splitmix64(&seed);

    for (u8 i = 0; i < 8; i++) zobrist_ep[i] = splitmix64(&seed);
}

u64 zobrist_hash(chessboard_t const* b) {
    u64 hash = 0;

    for (u8 sq = 0; sq < 64; sq++) {
        u64 mask = BB(sq);
        for (piece_t p = PAWN; p <= KING; p++) {
            for (color_t c = WHITE; c <= BLACK; c++) {
                if (pieces(b, p, c) & mask) hash ^= zobrist_pieces[p][c][sq];
            }
        }
    }

    if (b->st->side == BLACK) hash ^= zobrist_side;

    if (b->st->castling & 1) hash ^= zobrist_castling[0];
    if (b->st->castling & 2) hash ^= zobrist_castling[1];
    if (b->st->castling & 4) hash ^= zobrist_castling[2];
    if (b->st->castling & 8) hash ^= zobrist_castling[3];

    if (b->st->ep_sq != EN_PASSANT_NONE) hash ^= zobrist_ep[b->st->ep_sq & 7];

    return hash;
}
