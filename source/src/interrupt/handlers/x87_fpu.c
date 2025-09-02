#include <interrupt/interrupt_registry.h>

void x87_fpu_handler(void) {
    interrupt_registry_invoke(IC_X87_FPU);
}
