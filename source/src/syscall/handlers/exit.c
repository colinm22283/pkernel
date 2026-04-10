#include <scheduler/scheduler.h>

#include <syscall/handlers/exit.h>

#include <util/heap/heap.h>

#include <debug/printf.h>

void syscall_exit(__MAYBE_UNUSED uint64_t exit_code) {
    process_t * current_process = scheduler_current_process();

    process_kill(current_process);

    heap_overview();
    printf("%i/%i\n", heap_usage(), HEAP_INITIAL_SIZE);
}
