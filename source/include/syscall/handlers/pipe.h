#pragma once

#include <stdint.h>

#include <sys/types.h>

#include <defs.h>

int64_t syscall_pipe(fd_t * _path, open_options_t options);
