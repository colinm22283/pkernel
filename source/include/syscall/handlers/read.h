#pragma once

#include <stdint.h>

#include <pkos/defs.h>

#include <defs.h>

int64_t syscall_read(fd_t fd, char * _buffer, uint64_t size);