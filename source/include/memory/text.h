#pragma once

extern char _text_start;
#define TEXT_START ((void *) &_text_start)

extern char _text_end;
#define TEXT_END ((void *) &_text_end)

extern char _text_size;
#define TEXT_SIZE ((uint64_t) &_text_size)