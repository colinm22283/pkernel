#include <update.h>
#include <font.h>
#include <vga_terminal.h>

void terminal_update(void) {
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