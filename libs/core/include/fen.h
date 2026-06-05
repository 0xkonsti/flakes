#pragma once

#include "board.h"

#define FEN_INVALID 0xFF

static u8 const piece_from_fen[256] = {
    ['P'] = PAWN, ['p'] = PAWN, ['N'] = KNIGHT, ['n'] = KNIGHT, ['B'] = BISHOP, ['b'] = BISHOP,
    ['R'] = ROOK, ['r'] = ROOK, ['Q'] = QUEEN,  ['q'] = QUEEN,  ['K'] = KING,   ['k'] = KING,
};
static u8 const color_from_fen[256] = {
    ['P'] = WHITE, ['N'] = WHITE, ['B'] = WHITE, ['R'] = WHITE, ['Q'] = WHITE, ['K'] = WHITE,
    ['p'] = BLACK, ['n'] = BLACK, ['b'] = BLACK, ['r'] = BLACK, ['q'] = BLACK, ['k'] = BLACK,
};

u8 set_chessboard_from_fen(chessboard_t* board, state_t* state, char const* fen);

void fen_of_chessboard_print(chessboard_t const* board, state_t const* state);
