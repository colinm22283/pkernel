#pragma once

#include <stdint.h>

#include <sys/types.h>

#include <defs.h>

fd_t syscall_socket(socket_domain_t domain, socket_type_t type, uint64_t protocol);
