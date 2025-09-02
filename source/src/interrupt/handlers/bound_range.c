#include <interrupt/interrupt_registry.h>

void bound_range_handler(void) {
    interrupt_registry_invoke(IC_BOUND_RANGE);
}
