#include <interrupt/interrupt_registry.h>

void invalid_opcode_handler(void) {
    interrupt_registry_invoke(IC_INVALID_OPCODE);
}
