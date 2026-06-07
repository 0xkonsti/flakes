#include "search.h"
#include <stdint.h>
#include "board.h"
#include "legal.h"
#include "move.h"
#include "platform.h"
#include "static.h"

static u64 _nodes;

u64 search_nodes(void) {
    return _nodes;
}
void search_reset_nodes(void) {
    _nodes = 0;
}

// TODO: when in check, generate all evasions — stand-pat eval is invalid
static i32 _quiescence(chessboard_t* board, i32 alpha, i32 beta) {
    _nodes++;
    i32 static_eval = evaluate_v1(board);

    i32 best_val = static_eval;
    if (best_val >= beta) return best_val;
    if (best_val > alpha) alpha = best_val;

    movelist_t captures = get_all_legal_captures(board);
    for (usize i = 0; i < captures.count; i++) {
        move_t current = captures.moves[i];
        chessboard_make(board, current);
        i32 score = -_quiescence(board, -beta, -alpha);
        chessboard_unmake(board, current);

        if (score >= beta) return score;
        if (score > best_val) best_val = score;
        if (score > alpha) alpha = score;
    }

    return best_val;
}

i32 negamax(chessboard_t* board, u8 depth, i32 alpha, i32 beta) {
    _nodes++;
    if (depth == 0) return _quiescence(board, alpha, beta);

    movelist_t* legal = get_all_legal_moves(board);

    if (legal->count == 0) {
        u8 king_sq = bitscan_forward(board->kings[board->st->side]);
        // (MAX_PLAY - depth) : pref on earlyer mates
        if (chessboard_is_square_attacked(board, king_sq, !board->st->side)) return -MATE_SCORE + (MAX_PLY - depth);
        return 0;
    }

    for (usize i = 0; i < legal->count; i++) {
        move_t current = legal->moves[i];

        chessboard_make(board, current);
        i32 score = -negamax(board, depth - 1, -beta, -alpha);
        chessboard_unmake(board, current);

        if (score >= beta) return score;
        if (score > alpha) alpha = score;
    }

    return alpha;
}
