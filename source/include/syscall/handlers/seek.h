#pragma once

#include <stdint.h>

#include <pkos/defs.h>
#include <pkos/syscalls.h>

#include <defs.h>

int64_t syscall_seek(fd_t fd, int64_t offset, seek_origin_t origin);