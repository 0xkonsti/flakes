#include "board.h"
#include <assert.h>
#include <stdio.h>
#include "fen.h"
#include "move.h"
#include "zobrist.h"

static piece_t const promo_pieces[] = {QUEEN, KNIGHT, BISHOP, ROOK};

static void _xor_bb(chessboard_t* b, color_t c, piece_t p, u64 mask) {
    switch (p) {
        case PAWN:   b->pawns[c] ^= mask; break;
        case KNIGHT: b->knights[c] ^= mask; break;
        case BISHOP: b->bishops[c] ^= mask; break;
        case ROOK:   b->rooks[c] ^= mask; break;
        case QUEEN:  b->queens[c] ^= mask; break;
        case KING:   b->kings[c] ^= mask; break;
        default:     break;
    }
}

static u8 _ep_capture_sq(u8 to, color_t side) {
    return (u8)(side == WHITE ? to - 8 : to + 8);
}

static u64 _castling_rook_mask(u8 king_to, u8* rook_from, u8* rook_to) {
    if (king_to == SQ_G1) {
        *rook_from = SQ_H1;
        *rook_to = SQ_F1;
    } else if (king_to == SQ_C1) {
        *rook_from = SQ_A1;
        *rook_to = SQ_D1;
    } else if (king_to == SQ_G8) {
        *rook_from = SQ_H8;
        *rook_to = SQ_F8;
    } else {
        *rook_from = SQ_A8;
        *rook_to = SQ_D8;
    }
    return BB(*rook_from) | BB(*rook_to);
}

static void _update_castling_rights(chessboard_t* b, color_t side, piece_t moving, piece_t captured, u8 from, u8 to) {
    if (moving == KING) {
        if (side == WHITE)
            b->st->castling &= ~0x3;
        else
            b->st->castling &= ~0xC;
    }
    if (from == SQ_A1 || to == SQ_A1) b->st->castling &= ~0x2;
    if (from == SQ_H1 || to == SQ_H1) b->st->castling &= ~0x1;
    if (from == SQ_A8 || to == SQ_A8) b->st->castling &= ~0x8;
    if (from == SQ_H8 || to == SQ_H8) b->st->castling &= ~0x4;
    if (captured == ROOK) {
        if (to == SQ_A1) b->st->castling &= ~0x2;
        if (to == SQ_H1) b->st->castling &= ~0x1;
        if (to == SQ_A8) b->st->castling &= ~0x8;
        if (to == SQ_H8) b->st->castling &= ~0x4;
    }
}

void chessboard_make(chessboard_t* b, move_t m) {
    u8 from = MOV_FROM(m);
    u8 to = MOV_TO(m);
    u8 flags = MOV_FLAGS(m);
    piece_t moving = (piece_t)MOV_MOVING(m);
    piece_t captured = (piece_t)MOV_CAPTURED(m);

    assert(b->st + 1 < b->base_st + MAX_PLY);
    *(b->st + 1) = *b->st;
    b->st++;
    b->st->_lm_valid = false;

    color_t side = b->st->side;
    color_t opp = !side;
    u64 from_bb = BB(from);
    u64 to_bb = BB(to);

    if (flags == MF_PROMOTION) {
        piece_t promo = promo_pieces[MOV_PROMO(m)];
        _xor_bb(b, side, PAWN, from_bb);
        _xor_bb(b, side, promo, to_bb);
    } else {
        _xor_bb(b, side, moving, from_bb | to_bb);
    }

    if (captured) {
        u8 cap_sq = (flags == MF_ENPASSANT) ? _ep_capture_sq(to, side) : to;
        u64 cap_bb = BB(cap_sq);
        _xor_bb(b, opp, captured, cap_bb);
        b->occupied[opp] ^= cap_bb;
        b->all ^= cap_bb;
    }

    if (flags == MF_CASTLING) {
        u8 rf, rt;
        u64 rm = _castling_rook_mask(to, &rf, &rt);
        _xor_bb(b, side, ROOK, rm);
        b->occupied[side] ^= rm;
        b->all ^= rm;
    }

    b->occupied[side] ^= from_bb | to_bb;
    b->all ^= from_bb | to_bb;

    u8 old_castling = b->st->castling;
    u8 old_ep_sq = b->st->ep_sq;

    b->st->halfmove = (captured || moving == PAWN) ? 0 : b->st->halfmove + 1;
    b->st->ep_sq = EN_PASSANT_NONE;
    if (moving == PAWN && (to == from + 16 || to == from - 16)) {
        b->st->ep_sq = (u8)((from + to) / 2);
    }
    _update_castling_rights(b, side, moving, captured, from, to);
    b->st->side = opp;
    if (side == BLACK) b->st->fullmove++;

    b->st->hash ^= zobrist_piece_key(moving, side, from);
    b->st->hash ^= zobrist_piece_key(moving, side, to);
    if (captured != NO_PIECE) {
        u8 cap_sq = (flags == MF_ENPASSANT) ? _ep_capture_sq(to, side) : to;
        b->st->hash ^= zobrist_piece_key(captured, opp, cap_sq);
    }
    if (flags == MF_PROMOTION) {
        b->st->hash ^= zobrist_piece_key(PAWN, side, to);
        b->st->hash ^= zobrist_piece_key(promo_pieces[MOV_PROMO(m)], side, to);
    }
    b->st->hash ^= zobrist_side_key();
    b->st->hash ^= zobrist_castling_key(old_castling);
    b->st->hash ^= zobrist_castling_key(b->st->castling);
    b->st->hash ^= zobrist_ep_key(old_ep_sq);
    b->st->hash ^= zobrist_ep_key(b->st->ep_sq);
}

void chessboard_unmake(chessboard_t* b, move_t m) {
    assert(b->st > b->base_st);

    u8 from = MOV_FROM(m);
    u8 to = MOV_TO(m);
    u8 flags = MOV_FLAGS(m);
    piece_t moving = (piece_t)MOV_MOVING(m);
    piece_t captured = (piece_t)MOV_CAPTURED(m);

    // After make flipped the side, b->st->side is the next side to move.
    // So the side that just moved is !side.
    color_t mc = !b->st->side;
    color_t opp = b->st->side;
    u64 from_bb = BB(from);
    u64 to_bb = BB(to);

    // Reverse castling rook first (before king squares are restored)
    if (flags == MF_CASTLING) {
        u8 rf, rt;
        u64 rm = _castling_rook_mask(to, &rf, &rt);
        _xor_bb(b, mc, ROOK, rm);
        b->occupied[mc] ^= rm;
        b->all ^= rm;
    }

    // Restore captured piece
    if (captured) {
        u8 cap_sq = (flags == MF_ENPASSANT) ? _ep_capture_sq(to, mc) : to;
        u64 cap_bb = BB(cap_sq);
        _xor_bb(b, opp, captured, cap_bb);
        b->occupied[opp] ^= cap_bb;
        b->all ^= cap_bb;
    }

    // Clear destination, restore source
    if (flags == MF_PROMOTION) {
        piece_t promo = promo_pieces[MOV_PROMO(m)];
        _xor_bb(b, mc, promo, to_bb);
        _xor_bb(b, mc, PAWN, from_bb);
    } else {
        _xor_bb(b, mc, moving, from_bb | to_bb);
    }

    b->occupied[mc] ^= from_bb | to_bb;
    b->all ^= from_bb | to_bb;

    b->st--;
}

void chessboard_clear(chessboard_t* b, state_t* st) {
    for (int i = 0; i < 2; i++) {
        b->pawns[i] = 0;
        b->knights[i] = 0;
        b->bishops[i] = 0;
        b->rooks[i] = 0;
        b->queens[i] = 0;
        b->kings[i] = 0;
        b->occupied[i] = 0;
    }
    b->all = 0;

    st->castling = 0;
    st->ep_sq = EN_PASSANT_NONE;
    st->halfmove = 0;
    st->fullmove = 1;
    st->side = WHITE;
    st->hash = 0;
    st->_lm_valid = false;

    b->st = st;
    b->base_st = st;
}

bool chessboard_set_fen(chessboard_t* b, state_t* st, char const* fen) {
    return set_chessboard_from_fen(b, st, fen) == 0;
}

static char _piece_to_char(piece_t p, color_t c) {
    if (p == NO_PIECE) {
        return '.';
    }

    char base_char;
    switch (p) {
        case PAWN:   base_char = 'P'; break;
        case KNIGHT: base_char = 'N'; break;
        case BISHOP: base_char = 'B'; break;
        case ROOK:   base_char = 'R'; break;
        case QUEEN:  base_char = 'Q'; break;
        case KING:   base_char = 'K'; break;
        default:     return '?';
    }

    if (c == BLACK) {
        base_char += ('a' - 'A');
    }

    return base_char;
}

void chessboard_print(chessboard_t const* b) {
    u8 idx;
    for (i8 rank = 7; rank >= 0; rank--) {
        printf("%i  |", rank + 1);
        for (u8 file = 0; file < 8; file++) {
            idx = ((u8)rank) * 8 + file;
            piece_t p = piece_on(b, idx);
            color_t c = color_on(b, idx);
            printf("%c ", _piece_to_char(p, c));
        }
        printf("\n");
    }
    printf("   +----------------");
    printf("\n    A B C D E F G H\n");
}

gamestate_t chessboard_gamestate(chessboard_t const* b);
