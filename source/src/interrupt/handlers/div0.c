#include <interrupt/interrupt_registry.h>

void div0_handler(void) {
    interrupt_registry_invoke(IC_DIV0);
}