#pragma once

#include <stdint.h>

#include <pkos/types.h>

#include <defs.h>

error_number_t syscall_mount(const char * _dst, const char * _src, const char * _fs, mount_options_t options, const char * _data);