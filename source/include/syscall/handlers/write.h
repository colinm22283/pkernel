#pragma once

#include <stdint.h>

#include <sys/types.h>

#include <defs.h>

int64_t syscall_write(fd_t fd, const char * _buffer, uint64_t size);