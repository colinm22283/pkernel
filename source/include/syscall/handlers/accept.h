#pragma once

#include <stdint.h>

#include <sys/types.h>

#include <defs.h>

fd_t syscall_accept(fd_t sock_fd);
