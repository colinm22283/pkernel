#pragma once

#include <process/process.h>

#include <error_number.h>

error_number_t load_program(process_t * process, fs_directory_entry_t * dirent);