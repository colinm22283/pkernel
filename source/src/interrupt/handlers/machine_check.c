#include <interrupt/interrupt_registry.h>

void machine_check_handler(void) {
    interrupt_registry_invoke(IC_MACHINE_CHECK);
}
