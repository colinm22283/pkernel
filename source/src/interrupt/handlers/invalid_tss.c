#include <interrupt/interrupt_registry.h>

void invalid_tss_handler(void) {
    interrupt_registry_invoke(IC_INVALID_TSS);
}
