#pragma once

#include <stdbool.h>
#include <stdint.h>

#define WINDOW_WIDTH (320 / 8)
#define WINDOW_HEIGHT (200 / 8)

#define BUFFER_HEIGHT (50)

#define CONSOLE_BUFFER (*console_buffer)

typedef char console_buffer_t[WINDOW_WIDTH][BUFFER_HEIGHT];

extern bool (* draw_bitmap)(uint8_t * bitmap, uint64_t x, uint64_t y, uint64_t w, uint64_t h);
extern bool (* draw_bitmap_transparent)(uint8_t * bitmap, uint64_t x, uint64_t y, uint64_t w, uint64_t h);
extern bool (* clear_rect)(uint64_t x, uint64_t y, uint64_t w, uint64_t h);
extern console_buffer_t * console_buffer;

extern volatile bool cursor_on;
extern uint8_t cur_x, cur_y, view_y, buffer_start;