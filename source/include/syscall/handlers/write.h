#pragma once

#include <stdint.h>

#include <pkos/defs.h>

#include <defs.h>

int64_t syscall_write(fd_t fd, const char * _buffer, uint64_t size);