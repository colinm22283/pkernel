#pragma once

#include <stdint.h>

extern char _modules_text;
#define MODULES_TEXT ((void *) &_modules_text)

extern char _modules_data;
#define MODULES_DATA ((void *) &_modules_data)

extern char _modules_end;
#define MODULES_END ((void *) &_modules_end)

#define MODULES_TEXT_SIZE ((uint64_t) MODULES_DATA - (uint64_t) MODULES_TEXT)
#define MODULES_DATA_SIZE ((uint64_t) MODULES_END - (uint64_t) MODULES_DATA)