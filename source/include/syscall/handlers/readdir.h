#pragma once

#include <stdint.h>

#include <sys/types.h>

#include <defs.h>

int64_t syscall_readdir(fd_t fd, directory_entry_t * _entries, uint64_t size);