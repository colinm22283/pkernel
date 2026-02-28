#pragma once

#include <stdint.h>

#include <sys/types.h>

#include <defs.h>

int64_t syscall_chdir(const char * _path);