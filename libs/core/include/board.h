#pragma once

#include "move.h"
#include "move_lut.h"
#include "square.h"
#include "types.h"

#define BB(sq) (1ULL << (sq))

#define EN_PASSANT_NONE 64

#define MAX_PLY 512

static inline u8 sq_rank(u8 sq) {
    return sq >> 3;
}
static inline u8 sq_file(u8 sq) {
    return sq & 7;
}

typedef enum { WHITE, BLACK } color_t;

typedef enum { NO_PIECE, PAWN, KNIGHT, BISHOP, ROOK, QUEEN, KING } piece_t;

// State for undo / draw detection
typedef struct {
    u8 castling;  // bit flags: 0b0001=WK, 0b0010=WQ, 0b0100=BK, 0b1000=BQ
    u8 ep_sq;
    u8 halfmove;
    u16 fullmove;
    color_t side;
    u64 hash;  // zobrist hash for repetition detection
} state_t;

// chessboard_t now references a state stack
typedef struct {
    u64 pawns[2];
    u64 knights[2];
    u64 bishops[2];
    u64 rooks[2];
    u64 queens[2];
    u64 kings[2];
    u64 occupied[2];
    u64 all;

    state_t* st;       // current state
    state_t* base_st;  // 0 index of state stack
} chessboard_t;

// Make / unmake
void chessboard_make(chessboard_t* b, move_t m);
void chessboard_unmake(chessboard_t* b, move_t m);

// Initialisation
void chessboard_clear(chessboard_t* b, state_t* st);
bool chessboard_set_fen(chessboard_t* b, state_t* st, char const* fen);

void chessboard_print(chessboard_t const* b);

// statics
static inline chessboard_t chessboard_new(state_t* st) {
    chessboard_t b;
    chessboard_clear(&b, st);
    move_lut_init();
    return b;
}

static inline u64 pieces(chessboard_t const* b, piece_t p, color_t c) {
    switch (p) {
        case PAWN:   return b->pawns[c];
        case KNIGHT: return b->knights[c];
        case BISHOP: return b->bishops[c];
        case ROOK:   return b->rooks[c];
        case QUEEN:  return b->queens[c];
        case KING:   return b->kings[c];
        default:     return 0;
    }
}

static inline u64 color_bb(chessboard_t const* b, color_t c) {
    return b->occupied[c];
}

static inline piece_t piece_on(chessboard_t const* b, u8 sq) {
    u64 mask = BB(sq);
    if (b->pawns[WHITE] & mask || b->pawns[BLACK] & mask) return PAWN;
    if (b->knights[WHITE] & mask || b->knights[BLACK] & mask) return KNIGHT;
    if (b->bishops[WHITE] & mask || b->bishops[BLACK] & mask) return BISHOP;
    if (b->rooks[WHITE] & mask || b->rooks[BLACK] & mask) return ROOK;
    if (b->queens[WHITE] & mask || b->queens[BLACK] & mask) return QUEEN;
    if (b->kings[WHITE] & mask || b->kings[BLACK] & mask) return KING;
    return NO_PIECE;
}

static inline color_t color_on(chessboard_t const* b, u8 sq) {
    u64 mask = BB(sq);
    if (b->occupied[WHITE] & mask) return WHITE;
    return BLACK;
}

static inline void chessboard_set_piece(chessboard_t* b, color_t c, piece_t p, u8 sq) {
    u64 mask = BB(sq);
    switch (p) {
        case PAWN:   b->pawns[c] |= mask; break;
        case KNIGHT: b->knights[c] |= mask; break;
        case BISHOP: b->bishops[c] |= mask; break;
        case ROOK:   b->rooks[c] |= mask; break;
        case QUEEN:  b->queens[c] |= mask; break;
        case KING:   b->kings[c] |= mask; break;
        default:     return;
    }
    b->occupied[c] |= mask;
    b->all |= mask;
}
