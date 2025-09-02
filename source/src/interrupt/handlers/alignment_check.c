#include <interrupt/interrupt_registry.h>

void alignment_check_handler(void) {
    interrupt_registry_invoke(IC_ALIGNMENT_CHECK);
}
