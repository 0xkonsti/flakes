#include "algebraic.h"
#include <stdio.h>
#include "board.h"
#include "move.h"

#define MOVING_INDICATOR " PNBRQK"

// direct print to avoid heap allocations
void algebraic_from_move_print(move_t const m, chessboard_t const* b) {
    u8 from = MOV_FROM(m);
    u8 to = MOV_TO(m);
    u8 flags = MOV_FLAGS(m);
    u8 promo = MOV_PROMO(m);
    u8 moving = MOV_MOVING(m);
    u8 captured = MOV_CAPTURED(m);

    if (flags == MF_CASTLING) {
        if (to == SQ_G1 || to == SQ_G8) {
            printf("O-O");
        } else {
            printf("O-O-O");
        }
        return;
    }

    if (moving != PAWN) {
        printf("%c", MOVING_INDICATOR[moving]);
    } else if (captured) {
        printf("%c", 'a' + (from % 8));
    }

    // TODO: disambiguation

    if (captured) {
        printf("x");
    }

    printf("%c%d", 'a' + (to % 8), (to / 8) + 1);

    if (flags == MF_PROMOTION) {
        printf("=%c", MOVING_INDICATOR[promo]);
    }

    chessboard_t copy = *b;
    chessboard_make(&copy, m);

    gamestate_t gs = chessboard_gamestate(&copy);
    if (gs == GS_CHECK) {
        printf("+");
    } else if (gs == GS_CHECKMATE) {
        printf("#");
    }

    chessboard_unmake(&copy, m);
}
