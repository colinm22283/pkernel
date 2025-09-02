#include <interrupt/interrupt_registry.h>

void segment_not_present_handler(void) {
    interrupt_registry_invoke(IC_SEGMENT_NOT_PRESENT);
}
