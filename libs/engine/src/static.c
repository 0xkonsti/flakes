#include "static.h"
#include "board.h"

static i32 _material_value(chessboard_t* board) {
    return 0;
}

i32 evaluate_v1(chessboard_t* board) {
    return _material_value(board);
}
