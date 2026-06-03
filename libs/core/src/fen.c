#include "fen.h"
#include "utils.h"
#include "zobrist.h"

#define FEN_ERROR 0xFEFE

#define PARSE_STEP(fn, ...)           \
    do {                              \
        fen_index = fn(__VA_ARGS__);  \
        if (fen_index == FEN_ERROR) { \
            return FEN_INVALID;       \
        }                             \
    } while (0)

static usize _parse_fen_pieces(chessboard_t* b, char const* fen, usize fen_index) {
    for (u8 rank = 0; rank < 8; rank++) {
        u8 file = 0;

        while (file < 8) {
            char c = fen[fen_index];

            if (c >= '1' && c <= '8') {
                file += c - '0';
            } else {
                piece_t p = piece_from_fen[(u8)c];
                color_t col = color_from_fen[(u8)c];
                if (p == NO_PIECE) return FEN_ERROR;
                chessboard_set_piece(b, col, p, (7 - rank) * 8 + file);
                file++;
            }

            fen_index++;
        }

        if (rank < 7) {
            if (fen[fen_index] != '/') return FEN_ERROR;
            fen_index++;
        }
    }

    if (fen[fen_index] != ' ') return FEN_ERROR;

    return fen_index + 1;
}

static usize _parse_to_move(state_t* st, char const* fen, usize fen_index) {
    switch (fen[fen_index]) {
        case 'w': st->side = WHITE; break;
        case 'b': st->side = BLACK; break;
        default:  return FEN_ERROR;
    }

    if (fen[fen_index + 1] != ' ') {
        return FEN_ERROR;
    }

    return fen_index + 2;
}

static usize _parse_castling_rights(state_t* st, char const* fen, usize fen_index) {
    if (fen[fen_index] == '-') {
        if (fen[fen_index + 1] != ' ') return FEN_ERROR;
        return fen_index + 2;
    } else {
        u8 seen = 0;
        while (fen[fen_index] != ' ') {
            switch (fen[fen_index]) {
                case 'K':
                    if (seen & 0x1) return FEN_ERROR;
                    seen |= 0x1;
                    break;
                case 'Q':
                    if (seen & 0x2) return FEN_ERROR;
                    seen |= 0x2;
                    break;
                case 'k':
                    if (seen & 0x4) return FEN_ERROR;
                    seen |= 0x4;
                    break;
                case 'q':
                    if (seen & 0x8) return FEN_ERROR;
                    seen |= 0x8;
                    break;
                default: return FEN_ERROR;
            }
            fen_index++;
        }
        st->castling = seen;
    }

    return fen_index + 1;
}

static usize _parse_en_passant(state_t* st, char const* fen, usize fen_index) {
    if (fen[fen_index] == '-') return fen[fen_index + 1] == ' ' ? fen_index + 2 : FEN_ERROR;

    u8 file = (u8)fen[fen_index];
    if (file < 'a' || file > 'h') return FEN_ERROR;

    u8 rank = (u8)fen[fen_index + 1];
    if (rank < '1' || rank > '8') return FEN_ERROR;
    if (fen[fen_index + 2] != ' ') return FEN_ERROR;

    st->ep_sq = (rank - '1') * 8 + (file - 'a');

    return fen_index + 3;
}

static usize _parse_halfmove_clock(state_t* st, char const* fen, usize fen_index) {
    u8 value = 0;
    while (fen[fen_index] >= '0' && fen[fen_index] <= '9') value = (u8)(st->halfmove * 10 + (fen[fen_index++] - '0'));

    st->halfmove = value;

    if (fen[fen_index] != ' ') {
        return FEN_ERROR;
    }

    return fen_index + 1;
}

static usize _parse_fullmove_number(state_t* st, char const* fen, usize fen_index) {
    u16 value = 0;
    while (fen[fen_index] >= '0' && fen[fen_index] <= '9') value = (u16)(value * 10 + (fen[fen_index++] - '0'));

    st->fullmove = value;

    if (fen[fen_index] != '\0') {
        return FEN_ERROR;
    }

    return fen_index;
}

u8 set_chessboard_from_fen(chessboard_t* b, state_t* st, char const* fen) {
    if (fen == NULL) return FEN_INVALID;

    chessboard_clear(b, st);
    zobrist_init();

    usize fen_index = 0;

    PARSE_STEP(_parse_fen_pieces, b, fen, fen_index);
    PARSE_STEP(_parse_to_move, st, fen, fen_index);
    PARSE_STEP(_parse_castling_rights, st, fen, fen_index);
    PARSE_STEP(_parse_en_passant, st, fen, fen_index);
    PARSE_STEP(_parse_halfmove_clock, st, fen, fen_index);
    PARSE_STEP(_parse_fullmove_number, st, fen, fen_index);

    b->st = st;
    st->hash = zobrist_hash(b);

    return 0;
}
