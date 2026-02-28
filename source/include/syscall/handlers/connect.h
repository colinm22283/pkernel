#pragma once

#include <stdint.h>

#include <sys/types.h>

#include <defs.h>

int syscall_connect(fd_t sock_fd, const sockaddr_t * sockaddr, size_t sockaddr_len);
