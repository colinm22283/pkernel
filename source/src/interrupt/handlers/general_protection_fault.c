#include <interrupt/interrupt_registry.h>

void general_protection_fault_handler(void) {
    interrupt_registry_invoke(IC_GENERAL_PROTECTION_FAULT);
}
