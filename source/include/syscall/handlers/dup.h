#pragma once

#include <stdint.h>

#include <sys/types.h>

int64_t syscall_dup(fd_t dst, fd_t src);