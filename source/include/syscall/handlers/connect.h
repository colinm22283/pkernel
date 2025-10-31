#pragma once

#include <stdint.h>

#include <pkos/types.h>

#include <defs.h>

error_number_t syscall_connect(fd_t sock_fd, const sockaddr_t * sockaddr, size_t sockaddr_len);
