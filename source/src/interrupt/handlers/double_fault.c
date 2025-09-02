#include <interrupt/interrupt_registry.h>

void double_fault_handler(void) {
    interrupt_registry_invoke(IC_DOUBLE_FAULT);
}
