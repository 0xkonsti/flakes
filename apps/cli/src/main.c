#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include "algebraic.h"
#include "core.h"
#include "legal.h"
#include "options.h"
#include "types.h"
#include "utils.h"

#define BASE_FEN "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1"

static int read(char* buffer, usize b_size) {
    if (b_size == 0) {
        fprintf(stderr, "error: buffer size must be greater than 0\n");
        return 1;
    }

    if (buffer == NULL) {
        fprintf(stderr, "error: buffer cannot be NULL\n");
        return 1;
    }

    if (b_size > INT64_MAX) {
        fprintf(stderr, "error: buffer size exceeds maximum allowed size\n");
        return 1;
    }

    if (!fgets(buffer, (int)b_size, stdin)) {
        fprintf(stderr, "error: failed to read input\n");
        return 1;
    };

    // flush stdin if input exceeds buffer size
    if (strchr(buffer, '\n') == NULL) {
        int c;
        while ((c = getchar()) != '\n' && c != EOF);
    } else {
        buffer[strcspn(buffer, "\n")] = '\0';
    }

    return 0;
}

static int cmd_help(int argc, char** argv);
static int cmd_play(int argc, char** argv);

static struct {
    char const* name;
    char const* desc;
    int (*fn)(int, char**);
} cmds[] = {
    {"help", "Show this help message", cmd_help},
    {"play", "Play a game", cmd_play},
};

static int usage(void) {
    printf("Usage: flakes <command> [options]\n\n");
    printf("Commands:\n");
    for (usize i = 0; i < ARRAY_LEN(cmds); i++) {
        printf("  %-12s %s\n", cmds[i].name, cmds[i].desc);
    }
    printf("\nOptions:\n");
    printf(
        "  -f, --fen FEN  Set initial board position (FEN string)[Defaults to standard chessboard starting point]\n"
    );
    return 0;
}

static int cmd_help(int argc, char** argv) {
    UNUSED(argc);
    UNUSED(argv);
    return usage();
}

static int cmd_play(int argc, char** argv) {
    state_t st[MAX_PLY];
    chessboard_t b = chessboard_new(st);

    options_t options = options_parse(argc, argv);
    if (!options.valid) {
        return 1;
    }

    char const* fen = BASE_FEN;
    if (option_is_set(&options.fen)) {
        fen = option_get(&options.fen);
    }

    if (!chessboard_set_fen(&b, st, fen)) {
        printf("error: invalid FEN: '%s'\n", fen);
        return 1;
    }

    while (1) {  // Game Loop
        chessboard_print(&b);
        printf("\nIts %s's turn!\n", b.st->side == WHITE ? "White" : "Black");

        while (1) {  // Input Loop
            char sq_in[4];
            printf("Please select a Square (e.g. E2): ");
            if (read(sq_in, sizeof(sq_in)) != 0) {
                printf("error: failed to read input\n");
                continue;
            }

            u8 sq_idx = square_from_algebraic(sq_in);
            if (sq_idx == ALGEBRAIC_INVALID) {
                printf("Input is not an valid Square in Algebraic Notaion!\n");
                continue;
            }

            printf("square: %s = %u\n", sq_in, sq_idx);

            movelist_t legal = get_legal_moves(&b, sq_idx);

            printf("legal move count for [%s]: %u\n", sq_in, legal.count);
            for (usize i = 0; i < legal.count; i++) {
                printf("  %zu: ", i + 1);
                algebraic_from_move_print(legal.moves[i], &b);
                printf("\n");
            }

            printf("Select a move (1-%u): ", legal.count);
            char move_in[16];
            if (read(move_in, sizeof(move_in)) != 0) {
                printf("error: failed to read input\n");
                continue;
            }

            usize move_idx = strtoul(move_in, NULL, 10);
            if (move_idx == 0 || move_idx > legal.count) {
                printf("Input is not a valid move index!\n");
                continue;
            }

            move_t m = legal.moves[move_idx - 1];
            chessboard_make(&b, m);
            break;
        }
    }

    return 0;
}

int main(int argc, char** argv) {
    if (argc < 2) {
        return usage();
    }

    if (strcmp(argv[1], "-h") == 0 || strcmp(argv[1], "--help") == 0) {
        return usage();
    }

    for (usize i = 0; i < ARRAY_LEN(cmds); i++) {
        if (strcmp(argv[1], cmds[i].name) == 0) {
            return cmds[i].fn(argc - 1, argv + 1);
        }
    }

    printf("error: unknown command '%s'\n", argv[1]);
    return usage();
}
