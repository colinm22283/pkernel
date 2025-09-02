#include <interrupt/interrupt_registry.h>

void stack_segment_fault_handler(void) {
    interrupt_registry_invoke(IC_STACK_SEGMENT_FAULT);
}
