#pragma once

#include <stdint.h>

extern uint8_t font_basic[128][8];
extern uint8_t font_not_found[8];

static inline uint8_t * font_lookup(char c) {
    unsigned char uc = c;

    if (uc < 128) return font_basic[uc];
    else return font_not_found;
}