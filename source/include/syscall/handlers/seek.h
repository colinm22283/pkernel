#pragma once

#include <stdint.h>

#include <sys/types.h>

#include <defs.h>

int64_t syscall_seek(fd_t fd, int64_t offset, seek_origin_t origin);