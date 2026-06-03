#pragma once

#include "types.h"

typedef struct option_t {
    bool is_set;
    char const* value;
} option_t;

bool option_is_set(option_t const* self);
char const* option_get(option_t const* self);

typedef struct {
    bool valid;

    option_t fen;
} options_t;

options_t options_parse(int argc, char** argv);
