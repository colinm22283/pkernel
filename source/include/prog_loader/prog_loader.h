#pragma once

#include <process/process.h>

#include <errno.h>

int load_program(process_t * process, fs_directory_entry_t * dirent);