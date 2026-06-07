#pragma once

#include "board.h"
#include "move.h"
#include "types.h"

movelist_t get_legal_moves(chessboard_t* board, u8 square);

movelist_t* get_all_legal_moves(chessboard_t* board);

movelist_t get_all_legal_captures(chessboard_t* board);
