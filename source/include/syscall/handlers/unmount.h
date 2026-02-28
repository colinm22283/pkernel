#pragma once

#include <errno.h>

#include <sys/types.h>

int syscall_unmount(const char * _path);