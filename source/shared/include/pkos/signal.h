#pragma once

#include <stdint.h>

typedef uint32_t signal_number_t;

typedef void signal_handler_t(signal_number_t);

enum {
    SIG_ABORT,
    SIG_ALARM,

    SIG_PAGE,

    SIG_CHILD,

    _SIG_COUNT,
};


