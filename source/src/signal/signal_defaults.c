#include <signal/signal.h>

signal_action_t signal_defaults[_SIG_COUNT] = {
    [SIG_ABORT] = ACT_TERMINATE,
    [SIG_ALARM] = ACT_TERMINATE,
    [SIG_PAGE]  = ACT_TERMINATE,
    [SIG_CHILD] = ACT_IGNORE,
};
