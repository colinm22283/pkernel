#include <scheduler/core.h>

#include <util/heap/heap.h>

#include <sys/threading/hardware_concurrency.h>

size_t core_count;
scheduler_core_t * cores;

void scheduler_cores_init(void) {
    core_count = hardware_concurrency();

    cores = heap_alloc(core_count * sizeof(scheduler_core_t));

    for (size_t i = 0; i < core_count; i++) {
        cores[i].current_thread = NULL;
    }
}

scheduler_core_t * scheduler_current_core(void) {
    return &cores[0];
}
