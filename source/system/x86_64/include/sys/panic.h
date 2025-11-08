#pragma once

#include <stdint.h>
#include <stddef.h>

#include <defs.h>

extern uint8_t font_basic[128][8];
extern uint8_t font_not_found[8];

static inline uint8_t * font_lookup(char c) {
    unsigned char uc = c;

    if (uc < 128) return font_basic[uc];
    else return font_not_found;
}

__NORETURN void panic(
    const char * message,
    const char * label1, uint64_t value1,
    const char * label2, uint64_t value2,
    const char * label3, uint64_t value3
);

__NORETURN static inline void panic0(
    const char * message
) {
    panic(message, NULL, 0, NULL, 0, NULL, 0);
}

__NORETURN static inline void panic1(
    const char * message,
    const char * label1, uint64_t value1
) {
    panic(message, label1, value1, NULL, 0, NULL, 0);
}

__NORETURN static inline void panic2(
    const char * message,
    const char * label1, uint64_t value1,
    const char * label2, uint64_t value2
) {
    panic(message, label1, value1, label2, value2, NULL, 0);
}

__NORETURN static inline void panic3(
    const char * message,
    const char * label1, uint64_t value1,
    const char * label2, uint64_t value2,
    const char * label3, uint64_t value3
) {
    panic(message, label1, value1, label2, value2, label3, value3);
}
