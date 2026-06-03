#include "options.h"
#include <stdio.h>
#include <string.h>

static void _opt_set(option_t* self, char const* value) {
    self->is_set = true;
    self->value = value;
}

static options_t _options_init(void) {
    options_t options = (options_t){0};

    options.valid = true;

    return options;
}

bool option_is_set(option_t const* self) {
    return self->is_set;
}

char const* option_get(option_t const* self) {
    return self->value;
}

options_t options_parse(int argc, char** argv) {
    options_t options = _options_init();

    for (i64 i = 1; i < argc; i++) {
        if (strcmp(argv[i], "-f") == 0 || strcmp(argv[i], "--fen") == 0) {
            if (i + 1 < argc) {
                _opt_set(&options.fen, argv[++i]);
            } else {
                printf("error: --fen requires an argument\n");
                options.valid = false;
                break;
            }
        } else {
            printf("error: unknown option '%s'\n", argv[i]);
            options.valid = false;
            break;
        }
    }

    return options;
}
