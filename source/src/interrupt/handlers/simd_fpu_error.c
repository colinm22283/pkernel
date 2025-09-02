#include <interrupt/interrupt_registry.h>

void simd_fpu_error_handler(void) {
    interrupt_registry_invoke(IC_SIMD_FPU_ERROR);
}
