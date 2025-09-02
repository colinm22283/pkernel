#include <memory/primary_region.h>
#include <memory/memory_map.h>

uint64_t primary_region_start;
uint64_t primary_region_end;

void primary_region_init() {
    primary_memory_region_t primary_region = get_primary_memory_region();

    primary_region_start = primary_region.base;
    primary_region_end = primary_region.base + primary_region.length;
}