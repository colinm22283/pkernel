#pragma once

#include <paging/manager.h>

void sys_paging_map_kernel_regions(pman_context_t * context);
pman_mapping_t * sys_paging_map_kernel_executable(pman_context_t * context);
