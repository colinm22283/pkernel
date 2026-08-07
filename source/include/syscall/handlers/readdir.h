#pragma once

#include <stdint.h>

#include <sys/types.h>
#include <dirent.h>

#include <defs.h>

int syscall_readdir(fd_t fd, struct dirent * _entries, size_t size);
