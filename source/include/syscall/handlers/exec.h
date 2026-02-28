#pragma once

#include <process/process.h>

#include <errno.h>

int syscall_exec(const char * path, const char ** argv, uint64_t argc);
