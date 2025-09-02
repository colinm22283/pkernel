#include <interrupt/interrupt_registry.h>

void coproc_segment_overrun_handler(void) {
    interrupt_registry_invoke(IC_COPROC_SEGMENT_OVERRUN);
}
