#pragma once

#include <stdint.h>

#include <pkos/types.h>

#include <defs.h>

error_number_t syscall_listen(fd_t sock_fd, uint64_t size);
