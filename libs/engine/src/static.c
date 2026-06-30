#include "static.h"
#include "board.h"
#include "platform.h"

// https://en.wikipedia.org/wiki/Chess_piece_relative_value#Larry_Kaufman's_2021_system
static i32 _lk_2021(chessboard_t* board) {
    i32 score = 0;

    color_t side = board->st->side;

    u8 pawns_own = popcnt64(board->pawns[side]);
    u8 knights_own = popcnt64(board->knights[side]);
    u8 bishops_own = popcnt64(board->bishops[side]);
    u8 rooks_own = popcnt64(board->rooks[side]);
    u8 queens_own = popcnt64(board->queens[side]);

    u8 pawns_opp = popcnt64(board->pawns[!side]);
    u8 knights_opp = popcnt64(board->knights[!side]);
    u8 bishops_opp = popcnt64(board->bishops[!side]);
    u8 rooks_opp = popcnt64(board->rooks[!side]);
    u8 queens_opp = popcnt64(board->queens[!side]);

    score += (pawns_own - pawns_opp) * 100;
    score += (knights_own - knights_opp) * 320;
    score += (bishops_own - bishops_opp) * 330;
    score += (rooks_own - rooks_opp) * 500;
    score += (queens_own - queens_opp) * 950;

    return score;
}

static i32 _material_value(chessboard_t* board) {
    i32 value = _lk_2021(board);

    return value;
}

i32 evaluate_v1(chessboard_t* board) {
    return _material_value(board);
}
