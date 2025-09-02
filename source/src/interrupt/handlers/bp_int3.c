#include <interrupt/interrupt_registry.h>

void bp_int3_handler(void) {
    interrupt_registry_invoke(IC_BP_INT3);
}
