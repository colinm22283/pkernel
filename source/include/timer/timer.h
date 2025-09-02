#pragma once

#include <stdint.h>

#define TIMER_TICK_NS (54925)
#define TIMER_NS_TO_TICKS(ns) ((ns) / TIMER_TICK_NS)
#define TIMER_MS_TO_TICKS(ms) (TIMER_NS_TO_TICKS(1000 * (ms)))
#define TIMER_S_TO_TICKS(s) (TIMER_MS_TO_TICKS(1000 * (s)))

typedef enum {
    WAITING, RUNNING,
} timer_state_t;

struct timer_s;

typedef void (* timer_handler_t)(struct timer_s * timer);

typedef struct timer_s {
    timer_state_t state;

    uint64_t current_ticks;
    uint64_t start_ticks;
    uint64_t interval_ticks;

    void * cookie;
    timer_handler_t handler;

    struct timer_s * next;
    struct timer_s * prev;
} timer_t;

timer_t * timer_init(timer_handler_t handler, void * cookie, uint64_t start_ticks, uint64_t interval_ticks);
void timer_free(timer_t * timer);

static inline void * timer_get_cookie(timer_t * timer) { return timer->cookie; }

void timers_init(void);
void timers_update(void);