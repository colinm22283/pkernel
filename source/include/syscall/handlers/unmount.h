#pragma once

#include <error_number.h>

#include <pkos/types.h>

error_number_t syscall_unmount(const char * _path);