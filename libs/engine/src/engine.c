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

static void _swap_to_front(movelist_t* ml, move_t target) {
    for (usize i = 0; i < ml->count; i++) {
        if (ml->moves[i] == target) {
            ml->moves[0] ^= ml->moves[i];
            ml->moves[i] = ml->moves[0] ^ ml->moves[i];
            ml->moves[0] ^= ml->moves[i];
            return;
        }
    }
}

static movelist_t _preordered_movelist(movelist_t* legal);

move_t engine_search(engine_t* engine) {
    chessboard_t* board = engine->board;

    movelist_t* legal = get_all_legal_moves(board);
    move_t best_move = legal->moves[0];

    for (u8 depth = 1; depth <= engine->depth_limit; depth++) {
        search_reset_nodes();

        if (depth > 1) _swap_to_front(legal, best_move);

        i32 alpha = -INT32_MAX;

        for (usize i = 0; i < legal->count; i++) {
            move_t current = legal->moves[i];

            chessboard_make(board, current);
            i32 score = -negamax(board, depth - 1, -INT32_MAX, -alpha);
            chessboard_unmake(board, current);

            if (depth == engine->depth_limit) {
                algebraic_from_move_print(current, board);
                printf(": %d\n", score);
            }

            if (score > alpha) {
                alpha = score;
                best_move = current;
            }
        }

        printf("info depth %u score %d nodes %llu\n", depth, alpha, search_nodes());
    }

    return best_move;
}
