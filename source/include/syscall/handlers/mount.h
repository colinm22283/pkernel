#pragma once

#include <stdint.h>

#include <sys/types.h>

#include <defs.h>

int syscall_mount(const char * _dst, const char * _src, const char * _fs, mount_options_t options, const char * _data);