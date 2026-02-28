#pragma once

#include <stdint.h>

#include <sys/types.h>

#include <defs.h>

int syscall_listen(fd_t sock_fd, uint64_t size);
