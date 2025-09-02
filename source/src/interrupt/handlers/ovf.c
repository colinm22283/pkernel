#include <interrupt/interrupt_registry.h>

void ovf_handler(void) {
    interrupt_registry_invoke(IC_OVF);
}
