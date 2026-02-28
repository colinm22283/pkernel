#pragma once

#include <stdint.h>

#include <sys/types.h>

#include <defs.h>

int64_t syscall_open(const char * _path, open_options_t options);
int64_t syscall_openat(fd_t fd, const char * _path, open_options_t options);