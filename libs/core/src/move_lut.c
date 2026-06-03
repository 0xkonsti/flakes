#include "move_lut.h"
#include <stdlib.h>
#include "board.h"
#include "defines.h"
#include "platform.h"
#include "types.h"

u64 knight_move_lut[64];

u64 pawn_attack_lut[2][64];
u64 pawn_single_lut[2][64];
u64 pawn_double_lut[2][64];

u64 king_move_lut[64];

u64 bishop_mask[64];
u64 rook_mask[64];
u64* bishop_table[64];
u64* rook_table[64];

static u64* _bishop_data = NULL;
static u64* _rook_data = NULL;

// TODO: add some sort of looding indicator so show the prog of building luts on start up.

// all reachable squares in one direction (no occupancy, for mask)
static u64 _walk_mask(u8 sq, i8 step) {
    u64 m = 0;
    for (i8 n = (i8)sq + step; n >= 0 && n < 64; n += step) {
        m |= BB((u8)n);
        i8 next = n + step;
        if (next < 0 || next >= 64) break;  // off board
        u8 f_cur = sq_file((u8)n);
        u8 f_nxt = sq_file((u8)next);
        if (f_cur > f_nxt + 1u || f_cur + 1u < f_nxt) break;  // file wrap
    }
    return m;
}

// attack bitboard in one direction, stop at first blocker (inclusive)
static u64 _walk_attacks(u8 sq, i8 step, u64 occ) {
    u64 atk = 0;
    for (i8 n = (i8)sq + step; n >= 0 && n < 64; n += step) {
        atk |= BB((u8)n);            // include this square
        if (occ & BB((u8)n)) break;  // blocker → stop, capture allowed
        i8 next = n + step;
        if (next < 0 || next >= 64) break;
        u8 f_cur = sq_file((u8)n);
        u8 f_nxt = sq_file((u8)next);
        if (f_cur > f_nxt + 1u || f_cur + 1u < f_nxt) break;
    }
    return atk;
}

static void _pext_init(void) {
    static i8 const diag[] = {7, 9, -7, -9};
    static i8 const line[] = {1, -1, 8, -8};

    // Phase 1: build masks + calculate total table size
    usize b_total = 0, r_total = 0;
    for (u8 sq = 0; sq < 64; sq++) {
        bishop_mask[sq] = 0;
        rook_mask[sq] = 0;
        for (usize d = 0; d < 4; d++) {
            bishop_mask[sq] |= _walk_mask(sq, diag[d]);
            rook_mask[sq] |= _walk_mask(sq, line[d]);
        }
        b_total += 1ULL << popcnt64(bishop_mask[sq]);  // 2^popcnt entries per square
        r_total += 1ULL << popcnt64(rook_mask[sq]);
    }

    // Phase 2: allocate one flat buffer for all 64 tables
    _bishop_data = (u64*)malloc(b_total * sizeof(u64));
    _rook_data = (u64*)malloc(r_total * sizeof(u64));

    // Phase 3: fill tables
    u64* b_ptr = _bishop_data;
    u64* r_ptr = _rook_data;
    for (u8 sq = 0; sq < 64; sq++) {
        usize b_n = 1ULL << popcnt64(bishop_mask[sq]);
        usize r_n = 1ULL << popcnt64(rook_mask[sq]);

        bishop_table[sq] = b_ptr;  // point to this square's slice
        rook_table[sq] = r_ptr;

        // for every possible blocker pattern on this square's ray mask...
        for (usize i = 0; i < b_n; i++) {
            u64 occ = _pdep_u64(i, bishop_mask[sq]);  // scatter i's bits into mask positions
            u64 atk = 0;
            for (usize d = 0; d < 4; d++) atk |= _walk_attacks(sq, diag[d], occ);
            b_ptr[i] = atk;  // precomputed answer for this occupancy
        }
        b_ptr += b_n;

        for (usize i = 0; i < r_n; i++) {
            u64 occ = _pdep_u64(i, rook_mask[sq]);
            u64 atk = 0;
            for (usize d = 0; d < 4; d++) atk |= _walk_attacks(sq, line[d], occ);
            r_ptr[i] = atk;
        }
        r_ptr += r_n;
    }
}

static void _knight_init(void) {
    static i8 const offsets[] = {17, 15, 10, 6, -6, -10, -15, -17};
    for (u8 sq = 0; sq < 64; sq++) {
        u64 bb = 0;
        for (usize i = 0; i < ARRAY_LEN(offsets); i++) {
            i8 dst = (i8)sq + offsets[i];
            if (dst < 0 || dst >= 64) continue;
            u8 fd =
                (u8)(sq_file(sq) > sq_file((u8)dst) ? sq_file(sq) - sq_file((u8)dst) : sq_file((u8)dst) - sq_file(sq));
            if (fd == 1 || fd == 2) bb |= BB((u8)dst);
        }
        knight_move_lut[sq] = bb;
    }
}

static void _king_init(void) {
    static i8 const offsets[] = {-9, -8, -7, -1, 1, 7, 8, 9};
    for (u8 sq = 0; sq < 64; sq++) {
        u64 bb = 0;
        for (usize i = 0; i < ARRAY_LEN(offsets); i++) {
            i8 dst = (i8)sq + offsets[i];
            if (dst < 0 || dst >= 64) continue;
            u8 fd =
                (u8)(sq_file(sq) > sq_file((u8)dst) ? sq_file(sq) - sq_file((u8)dst) : sq_file((u8)dst) - sq_file(sq));
            if (fd <= 1) bb |= BB((u8)dst);
        }
        king_move_lut[sq] = bb;
    }
}

static void _pawn_init(void) {
    for (u8 sq = 0; sq < 64; sq++) {
        u8 r = sq_rank(sq);
        u8 f = sq_file(sq);

        // Single push
        if (r < 7) pawn_single_lut[WHITE][sq] = BB(sq + 8);
        if (r > 0) pawn_single_lut[BLACK][sq] = BB(sq - 8);

        // Double push (only from starting rank)
        if (r == 1) pawn_double_lut[WHITE][sq] = BB(sq + 16);
        if (r == 6) pawn_double_lut[BLACK][sq] = BB(sq - 16);

        // Captures
        pawn_attack_lut[WHITE][sq] = 0;
        if (f > 0 && r < 7) pawn_attack_lut[WHITE][sq] |= BB(sq + 7);
        if (f < 7 && r < 7) pawn_attack_lut[WHITE][sq] |= BB(sq + 9);

        pawn_attack_lut[BLACK][sq] = 0;
        if (f < 7 && r > 0) pawn_attack_lut[BLACK][sq] |= BB(sq - 7);
        if (f > 0 && r > 0) pawn_attack_lut[BLACK][sq] |= BB(sq - 9);
    }
}

void move_lut_init(void) {
    static bool done = false;
    if (done) return;

    _knight_init();
    _pawn_init();
    _king_init();
    _pext_init();

    done = true;
}
