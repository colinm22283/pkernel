#pragma once

#include <pkos/types.h>

void * syscall_map(fd_t fd, void * map_address, uint64_t size, uint64_t offset, map_options_t options);