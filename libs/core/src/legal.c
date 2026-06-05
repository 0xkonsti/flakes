#include "legal.h"
#include "board.h"
#include "defines.h"
#include "move_lut.h"
#include "platform.h"

static void _get_legal_pawn_moves(chessboard_t const* b, u8 sq, movelist_t* moves) {
    color_t side = b->st->side;
    u8 promo_rank = side == WHITE ? 7 : 0;

    u8 to;

    u64 single_bb = pawn_single_lut[side][sq];
    if (single_bb && !(b->all & single_bb)) {
        to = bitscan_forward(single_bb);
        if (sq_rank(to) == promo_rank) {
            movelist_add(moves, move_create(sq, to, MF_PROMOTION, KNIGHT, PAWN, NO_PIECE));
            movelist_add(moves, move_create(sq, to, MF_PROMOTION, BISHOP, PAWN, NO_PIECE));
            movelist_add(moves, move_create(sq, to, MF_PROMOTION, ROOK, PAWN, NO_PIECE));
            movelist_add(moves, move_create(sq, to, MF_PROMOTION, QUEEN, PAWN, NO_PIECE));
        } else {
            movelist_add(moves, move_create(sq, to, MF_NORMAL, NO_PIECE, PAWN, NO_PIECE));
        }

        u64 double_bb = pawn_double_lut[side][sq];
        if (double_bb && !(b->all & double_bb)) {
            to = bitscan_forward(double_bb);
            movelist_add(moves, move_create(sq, to, MF_NORMAL, 0, PAWN, NO_PIECE));
        }
    }

    u64 mask = b->occupied[!side];
    if (b->st->ep_sq != EN_PASSANT_NONE) BIT_SET(mask, b->st->ep_sq);
    u64 attacks = pawn_attack_lut[side][sq] & mask;
    while (attacks) {
        to = bitscan_forward(attacks);
        attacks &= attacks - 1;

        if (sq_rank(to) == promo_rank) {
            u8 captured = (u8)piece_on(b, to);

            movelist_add(moves, move_create(sq, to, MF_PROMOTION, KNIGHT, PAWN, captured));
            movelist_add(moves, move_create(sq, to, MF_PROMOTION, BISHOP, PAWN, captured));
            movelist_add(moves, move_create(sq, to, MF_PROMOTION, ROOK, PAWN, captured));
            movelist_add(moves, move_create(sq, to, MF_PROMOTION, QUEEN, PAWN, captured));
        } else {
            u8 captured;
            u8 flags;
            if (to == b->st->ep_sq) {
                captured = PAWN;
                flags = MF_ENPASSANT;
            } else {
                captured = (u8)piece_on(b, to);
                flags = MF_NORMAL;
            }

            movelist_add(moves, move_create(sq, to, flags, NO_PIECE, PAWN, captured));
        }
    }
}

static void _get_legal_knight_moves(chessboard_t const* b, u8 sq, movelist_t* moves) {
    color_t side = b->st->side;

    u64 targets = knight_move_lut[sq] & ~b->occupied[side];
    while (targets) {
        u8 to = bitscan_forward(targets);
        targets &= targets - 1;

        u8 captured = (u8)piece_on(b, to);

        movelist_add(moves, move_create(sq, to, MF_NORMAL, NO_PIECE, KNIGHT, captured));
    }
}

static void _get_legal_bishop_moves(chessboard_t const* b, u8 sq, movelist_t* moves) {
    color_t side = b->st->side;

    u64 targets = bishop_attacks(sq, b->all) & ~b->occupied[side];
    while (targets) {
        u8 to = bitscan_forward(targets);
        targets &= targets - 1;

        u8 captured = (u8)piece_on(b, to);
        movelist_add(moves, move_create(sq, to, MF_NORMAL, NO_PIECE, BISHOP, captured));
    }
}

static void _get_legal_rook_moves(chessboard_t const* b, u8 sq, movelist_t* moves) {
    color_t side = b->st->side;

    u64 targets = rook_attacks(sq, b->all) & ~b->occupied[side];
    while (targets) {
        u8 to = bitscan_forward(targets);
        targets &= targets - 1;

        u8 captured = (u8)piece_on(b, to);
        movelist_add(moves, move_create(sq, to, MF_NORMAL, NO_PIECE, ROOK, captured));
    }
}

static void _get_legal_queen_moves(chessboard_t const* b, u8 sq, movelist_t* moves) {
    color_t side = b->st->side;

    u64 targets = queen_attacks(sq, b->all) & ~b->occupied[side];
    while (targets) {
        u8 to = bitscan_forward(targets);
        targets &= targets - 1;

        u8 captured = (u8)piece_on(b, to);
        movelist_add(moves, move_create(sq, to, MF_NORMAL, NO_PIECE, QUEEN, captured));
    }
}

static void _get_legal_king_moves(chessboard_t const* b, u8 sq, movelist_t* moves) {
    color_t side = b->st->side;

    u64 targets = king_move_lut[sq] & ~b->occupied[side];
    while (targets) {
        u8 to = bitscan_forward(targets);
        targets &= targets - 1;

        u8 captured = (u8)piece_on(b, to);
        movelist_add(moves, move_create(sq, to, MF_NORMAL, NO_PIECE, KING, captured));
    }

    // Casteling: bit messy but does the job...
    if (!chessboard_is_square_attacked(b, sq, !side)) {
        u8 cr = b->st->castling;
        if (sq == SQ_E1 && side == WHITE) {
            if ((cr & 0x1) && !(b->all & (BB(SQ_F1) | BB(SQ_G1))) && !chessboard_is_square_attacked(b, SQ_F1, !side))
                movelist_add(moves, move_create(SQ_E1, SQ_G1, MF_CASTLING, 0, KING, NO_PIECE));
            if ((cr & 0x2) && !(b->all & (BB(SQ_B1) | BB(SQ_C1) | BB(SQ_D1))) &&
                !chessboard_is_square_attacked(b, SQ_D1, !side))
                movelist_add(moves, move_create(SQ_E1, SQ_C1, MF_CASTLING, 0, KING, NO_PIECE));
        }
        if (sq == SQ_E8 && side == BLACK) {
            if ((cr & 0x4) && !(b->all & (BB(SQ_F8) | BB(SQ_G8))) && !chessboard_is_square_attacked(b, SQ_F8, !side))
                movelist_add(moves, move_create(SQ_E8, SQ_G8, MF_CASTLING, 0, KING, NO_PIECE));
            if ((cr & 0x8) && !(b->all & (BB(SQ_B8) | BB(SQ_C8) | BB(SQ_D8))) &&
                !chessboard_is_square_attacked(b, SQ_D8, !side))
                movelist_add(moves, move_create(SQ_E8, SQ_C8, MF_CASTLING, 0, KING, NO_PIECE));
        }
    }
}

static void _gen_pseudo_legal(chessboard_t const* b, u8 sq, movelist_t* ml) {
    switch (piece_on(b, sq)) {
        case PAWN:     _get_legal_pawn_moves(b, sq, ml); break;
        case KNIGHT:   _get_legal_knight_moves(b, sq, ml); break;
        case BISHOP:   _get_legal_bishop_moves(b, sq, ml); break;
        case ROOK:     _get_legal_rook_moves(b, sq, ml); break;
        case QUEEN:    _get_legal_queen_moves(b, sq, ml); break;
        case KING:     _get_legal_king_moves(b, sq, ml); break;
        case NO_PIECE: break;
    }
}

static void _get_legal_moves(chessboard_t* b, u8 sq, movelist_t* ml) {
    color_t side = b->st->side;
    movelist_t pseudo = movelist_empty();
    if (color_on(b, sq) != side) {
        return;
    }
    _gen_pseudo_legal(b, sq, &pseudo);

    for (u8 i = 0; i < pseudo.count; i++) {
        chessboard_make(b, pseudo.moves[i]);

        u8 king_sq = bitscan_forward(b->kings[side]);
        if (!chessboard_is_square_attacked(b, king_sq, !side)) movelist_add(ml, pseudo.moves[i]);

        chessboard_unmake(b, pseudo.moves[i]);
    }
}

movelist_t get_legal_moves(chessboard_t* b, u8 sq) {
    movelist_t* legal = get_all_legal_moves(b);
    movelist_t filtered = movelist_empty();

    for (u8 i = 0; i < legal->count; i++) {
        if (MOV_FROM(legal->moves[i]) == sq) {
            movelist_add(&filtered, legal->moves[i]);
        }
    }

    return filtered;
}

movelist_t* get_all_legal_moves(chessboard_t* b) {
    if (b->st->_lm_valid) {
        return &b->st->_legal_moves;
    }

    color_t side = b->st->side;
    movelist_t legal = movelist_empty();

    u64 squares = b->occupied[side];
    while (squares) {
        u8 sq = bitscan_forward(squares);
        squares &= squares - 1;
        _get_legal_moves(b, sq, &legal);
    }

    b->st->_legal_moves = legal;
    b->st->_lm_valid = true;

    return &b->st->_legal_moves;
}
