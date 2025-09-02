#pragma once

#include <stdint.h>

extern char _data_start;
#define DATA_START ((void *) &_data_start)

extern char _data_end;
#define DATA_END ((void *) &_data_end)

extern char _data_size;
#define DATA_SIZE ((uint64_t) &_data_size)