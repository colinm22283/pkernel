#pragma once

#include <pman/types.h>

void sys_paging_map_kernel_regions(pman_context_t * context);
pman_range_t * sys_paging_map_kernel_executable(pman_context_t * context);
