#include <interrupt/interrupt_registry.h>

void nmi_handler(void) {
    interrupt_registry_invoke(IC_NMI);
}
