#pragma once

#include "board.h"
#include "move.h"

#define DEFAULT_DEPTH 6

typedef struct engine_t {
    chessboard_t* board;

    u8 depth_limit;
} engine_t;

void engine_init(engine_t* engine, chessboard_t* board);

move_t engine_search(engine_t* engine);
