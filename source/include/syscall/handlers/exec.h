#pragma once

#include <error_number.h>

error_number_t syscall_exec(const char * path, const char ** argv, uint64_t argc);
