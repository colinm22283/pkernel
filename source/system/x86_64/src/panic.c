#include <stddef.h>

#include <sys/panic.h>

#include <sys/halt.h>

#define VIDEO_BUFFER ((uint8_t *) 0xA0000)

enum {
    COLOR_BLACK = 0,
    COLOR_BLUE  = 1,
    COLOR_GREEN = 2,
    COLOR_RED   = 4,
    COLOR_WHITE = 15,
};

uint8_t fg_color = COLOR_BLACK;
uint8_t bg_color = COLOR_WHITE;

void draw_char(uint8_t * glyph, uint8_t row, uint8_t col) {
    for (uint8_t y = 0; y < 8; y++) {
        uint8_t r = glyph[y];

        for (uint8_t x = 0; x < 8; x++) {
            VIDEO_BUFFER[(row * 8 + y) * 320 + col * 8 + x] = (r & (1 << x)) ? fg_color : bg_color;
        }
    }
}

void clear_row(uint8_t row) {
    for (uint8_t y = 0; y < 8; y++) {
        for (uint16_t x = 0; x < 320; x++) {
            VIDEO_BUFFER[(row * 8 + y) * 320 + x] = bg_color;
        }
    }
}

void draw_string(const char * str, uint8_t row, uint8_t col) {
    for (uint8_t i = 0; str[i] != '\0'; i++) {
        draw_char(font_lookup(str[i]), row, col + i);
    }
}

void draw_hex(uint64_t num, uint8_t row, uint8_t col) {
    for (uint8_t i = 0; i < 16; i++) {
        draw_char(font_lookup((char) ('0' + ((num >> (i * 4)) & 0xF))), row, col + 15 - i);
    }
}

__NORETURN void panic(
    const char * message,
    const char * label1, uint64_t value1,
    const char * label2, uint64_t value2,
    const char * label3, uint64_t value3
) {
    cli();

    fg_color = COLOR_RED;
    bg_color = COLOR_WHITE;
    draw_string("KERNEL PANIC", 0, 0);

    fg_color = COLOR_WHITE;
    bg_color = COLOR_BLUE;
    draw_string(message, 1, 0);

    if (label1 != NULL) {
        clear_row(2);
        draw_string(label1, 2, 0);
        draw_string("0x", 2, 18);
        draw_hex(value1, 2, 20);
    }

    if (label2 != NULL) {
        clear_row(3);
        draw_string(label2, 3, 0);
        draw_string("0x", 3, 18);
        draw_hex(value2, 3, 20);
    }

    if (label3 != NULL) {
        clear_row(4);
        draw_string(label3, 4, 0);
        draw_string("0x", 4, 18);
        draw_hex(value3, 4, 20);
    }

    halt();
}
