#pragma once

#include <assert.h>
#include "board.h"
#include "move.h"
#include "types.h"

#define MAX_MOVE_COUNT 256

typedef struct {
    move_t moves[MAX_MOVE_COUNT];
    u8 count;
} movelist_t;

static inline movelist_t movelist_empty(void) {
    return (movelist_t){0};
}

static inline void movelist_add(movelist_t* movelist, move_t move) {
    assert(movelist->count < (MAX_MOVE_COUNT - 1));

    movelist->moves[movelist->count++] = move;
}

static inline void movelist_for_each(movelist_t const* movelist, void (*fn)(move_t const)) {
    for (u8 i = 0; i < movelist->count; i++) {
        fn(movelist->moves[i]);
    }
}

movelist_t get_legal_moves(chessboard_t* board, u8 square);

movelist_t get_all_legal_moves(chessboard_t* board);
