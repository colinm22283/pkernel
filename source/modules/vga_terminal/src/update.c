#include <stddef.h>

#include <update.h>
#include <font.h>
#include <vga_terminal.h>

bool clear_rect(uint64_t _x, uint64_t _y, uint64_t w, uint64_t h) {
    for (uint64_t x = _x; x < _x + w; x++) {
        for (uint64_t y = _y; y < _y + h; y++) {
            frame_buffer[y * 320 + x] = 0x00;
        }
    }

    return true;
}

bool draw_bitmap(const uint8_t * bitmap, uint64_t _x, uint64_t _y, uint64_t w, uint64_t h) {
    if (bitmap == NULL) return false;

    uint64_t byte_width = w / 8;

    for (uint64_t y = 0; y < h; y++) {
        for (uint64_t x = 0; x < w; x++) {
            frame_buffer[(_y + y) * 320 + _x + x] = ((bitmap[y * byte_width + x / 8] >> x) & 1) ? 0x0F : 0x00;
        }
    }

    return true;
}

bool draw_bitmap_transparent(const uint8_t * bitmap, uint64_t _x, uint64_t _y, uint64_t w, uint64_t h) {
    if (bitmap == NULL) return false;

    uint64_t byte_width = w / 8;

    for (uint64_t y = 0; y < h; y++) {
        for (uint64_t x = 0; x < w; x++) {
            if ((bitmap[y * byte_width + x / 8] >> x) & 1) frame_buffer[(_y + y) * 320 + _x + x] = 0x0F;
        }
    }

    return true;
}

void terminal_update(void) {
    if (!bound) return;

    for (uint8_t y = 0; y < WINDOW_HEIGHT; y++) {
        uint64_t buffer_y = (view_y - y + BUFFER_HEIGHT) % BUFFER_HEIGHT;

        if (
            (buffer_y < buffer_start && (
                (buffer_y <= view_y && buffer_y <= buffer_start) ||
                (buffer_y >= view_y && buffer_y >= buffer_start)
            )) ||
            (
                buffer_y > buffer_start
            )
        ) {
            for (uint8_t x = 0; x < WINDOW_WIDTH; x++) {
                uint64_t buffer_x = x;

                uint8_t * font_char = font_lookup(CONSOLE_BUFFER[buffer_x][buffer_y]);

                draw_bitmap(font_char, x * 8, (WINDOW_HEIGHT - y - 1) * 8, 8, 8);
                if (cur_x == x && cur_y == buffer_y) {
                    if (cursor_on) draw_bitmap_transparent(font_lookup('_'), x * 8, (WINDOW_HEIGHT - y - 1) * 8, 8, 8);
                }
            }
        }
        else {
            clear_rect(0, (WINDOW_HEIGHT - y - 1) * 8, WINDOW_WIDTH * 8, 8);
        }
    }
}