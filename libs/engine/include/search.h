#pragma once

#include "board.h"
#include "types.h"

#define MATE_SCORE 0x30C30

i32 negamax(chessboard_t* board, u8 depth, i32 alpha, i32 beta);

u64 search_nodes(void);

void search_reset_nodes(void);
