#include "engine.h"
#include <stdint.h>
#include <stdio.h>
#include "algebraic.h"
#include "board.h"
#include "legal.h"
#include "move.h"
#include "search.h"

void engine_init(engine_t* engine, chessboard_t* board) {
    engine->board = board;
    engine->depth_limit = DEFAULT_DEPTH;
}

move_t engine_search(engine_t* engine) {
    chessboard_t* board = engine->board;
    search_reset_nodes();

    movelist_t* legal = get_all_legal_moves(board);

    for (usize i = 0; i < legal->count; i++) {
        move_t current = legal->moves[i];
        algebraic_from_move_print(current, board);

        chessboard_make(board, current);

        i32 score = negamax(board, engine->depth_limit, INT32_MIN, INT32_MAX);

        printf(": %i\n", score);

        chessboard_unmake(board, current);
    }

    return 0;
}
