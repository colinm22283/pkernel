#pragma once

#include <stdint.h>

#include <sys/types.h>

#include <defs.h>

int syscall_signal(signal_number_t sig, signal_handler_t * handler);
