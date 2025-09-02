#include <stddef.h>

#include <filesystem/filesystem.h>

#include <interrupt/interrupt_registry.h>

#include <util/string/strlen.h>

#include <sys/asm/cli.h>
#include <sys/asm/hlt.h>

filesystem_node_t * tty_node;

const char * channel_names[] = {
    "DIV0",
    "NMI",
    "BP_INT3",
    "OVF",
    "BOUND_RANGE",
    "INVALID_OPCODE",
    "DEVICE_NOT_AVAIL",
    "DOUBLE_FAULT",
    "COPROC_SEGMENT_OVERRUN",
    "INVALID_TSS",
    "SEGMENT_NOT_PRESENT",
    "STACK_SEGMENT_FAULT",
    "GENERAL_PROTECTION_FAULT",
    "PAGE_FAULT",
    "X87_FPU",
    "ALIGNMENT_CHECK",
    "MACHINE_CHECK",
    "SIMD_FPU_ERROR",
};

bool exception_handler(interrupt_channel_t channel, void * cookie) {
    const char * str = "EX: ";
    filesystem_node_write(tty_node, str, strlen(str), 0);
    filesystem_node_write(tty_node, channel_names[channel], strlen(channel_names[channel]), 0);

    cli();
    hlt();

    return false;
}

bool init(void) {
    tty_node = filesystem_find(filesystem_root(), "dev/tty");

    if (!interrupt_registry_register(IC_DIV0, exception_handler, NULL)) return false;
    if (!interrupt_registry_register(IC_NMI, exception_handler, NULL)) return false;
    if (!interrupt_registry_register(IC_BP_INT3, exception_handler, NULL)) return false;
    if (!interrupt_registry_register(IC_OVF, exception_handler, NULL)) return false;
    if (!interrupt_registry_register(IC_BOUND_RANGE, exception_handler, NULL)) return false;
    if (!interrupt_registry_register(IC_INVALID_OPCODE, exception_handler, NULL)) return false;
    if (!interrupt_registry_register(IC_DEVICE_NOT_AVAIL, exception_handler, NULL)) return false;
    if (!interrupt_registry_register(IC_DOUBLE_FAULT, exception_handler, NULL)) return false;
    if (!interrupt_registry_register(IC_COPROC_SEGMENT_OVERRUN, exception_handler, NULL)) return false;
    if (!interrupt_registry_register(IC_INVALID_TSS, exception_handler, NULL)) return false;
    if (!interrupt_registry_register(IC_SEGMENT_NOT_PRESENT, exception_handler, NULL)) return false;
    if (!interrupt_registry_register(IC_STACK_SEGMENT_FAULT, exception_handler, NULL)) return false;
    if (!interrupt_registry_register(IC_GENERAL_PROTECTION_FAULT, exception_handler, NULL)) return false;
    if (!interrupt_registry_register(IC_PAGE_FAULT, exception_handler, NULL)) return false;
    if (!interrupt_registry_register(IC_X87_FPU, exception_handler, NULL)) return false;
    if (!interrupt_registry_register(IC_ALIGNMENT_CHECK, exception_handler, NULL)) return false;
    if (!interrupt_registry_register(IC_MACHINE_CHECK, exception_handler, NULL)) return false;
    if (!interrupt_registry_register(IC_SIMD_FPU_ERROR, exception_handler, NULL)) return false;

    return true;
}

bool free(void) {
    if (!interrupt_registry_free(IC_DIV0)) return false;
    if (!interrupt_registry_free(IC_NMI)) return false;
    if (!interrupt_registry_free(IC_BP_INT3)) return false;
    if (!interrupt_registry_free(IC_OVF)) return false;
    if (!interrupt_registry_free(IC_BOUND_RANGE)) return false;
    if (!interrupt_registry_free(IC_INVALID_OPCODE)) return false;
    if (!interrupt_registry_free(IC_DEVICE_NOT_AVAIL)) return false;
    if (!interrupt_registry_free(IC_DOUBLE_FAULT)) return false;
    if (!interrupt_registry_free(IC_COPROC_SEGMENT_OVERRUN)) return false;
    if (!interrupt_registry_free(IC_INVALID_TSS)) return false;
    if (!interrupt_registry_free(IC_SEGMENT_NOT_PRESENT)) return false;
    if (!interrupt_registry_free(IC_STACK_SEGMENT_FAULT)) return false;
    if (!interrupt_registry_free(IC_GENERAL_PROTECTION_FAULT)) return false;
    if (!interrupt_registry_free(IC_PAGE_FAULT)) return false;
    if (!interrupt_registry_free(IC_X87_FPU)) return false;
    if (!interrupt_registry_free(IC_ALIGNMENT_CHECK)) return false;
    if (!interrupt_registry_free(IC_MACHINE_CHECK)) return false;
    if (!interrupt_registry_free(IC_SIMD_FPU_ERROR)) return false;

    return true;
}