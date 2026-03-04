#pragma once

#include <stdint.h>

#include <sys/types.h>

#include <defs.h>

int syscall_mount(const char * _src, const char * _dst, const char * _fs, unsigned long options, const char * _data);
