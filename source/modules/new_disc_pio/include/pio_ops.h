#pragma once

#include <stdint.h>

typedef struct {

} disc_controller_t;

bool disc_read(uint64_t lba, uint64_t sector_count, void * dst);
bool disc_write(uint64_t lba, uint64_t sector_count, const void * src);
