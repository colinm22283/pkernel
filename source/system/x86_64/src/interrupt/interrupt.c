#include <interrupt/apic.h>

#include <syscall/syscall_handler_entry.h>

#include <sys/interrupt/interrupt.h>
#include <sys/interrupt/interrupt_entries.h>

#include <sys/pic/pic.h>

#include <sys/idt/idt.h>

#include <sys/gdt/gdt.h>

#include <sys/asm/lidt.h>
#include <sys/asm/cli.h>

void interrupt_init() {
    cli();

    idt.div0                     = DEFINE_IDT64_ENTRY_INTERRUPT(GDT_KERNEL_CODE, div0_handler_entry);
    idt.nmi                      = DEFINE_IDT64_ENTRY_INTERRUPT(GDT_KERNEL_CODE, nmi_handler_entry);
    idt.bp_int3                  = DEFINE_IDT64_ENTRY_INTERRUPT(GDT_KERNEL_CODE, bp_int3_handler_entry);
    idt.ovf                      = DEFINE_IDT64_ENTRY_INTERRUPT(GDT_KERNEL_CODE, ovf_handler_entry);
    idt.bound_range              = DEFINE_IDT64_ENTRY_INTERRUPT(GDT_KERNEL_CODE, bound_range_handler_entry);
    idt.invalid_opcode           = DEFINE_IDT64_ENTRY_INTERRUPT(GDT_KERNEL_CODE, invalid_opcode_handler_entry);
    idt.device_not_avail         = DEFINE_IDT64_ENTRY_INTERRUPT(GDT_KERNEL_CODE, device_not_avail_handler_entry);
    idt.double_fault             = DEFINE_IDT64_ENTRY_INTERRUPT(GDT_KERNEL_CODE, double_fault_handler_entry);
    idt.coproc_segment_overrun   = DEFINE_IDT64_ENTRY_INTERRUPT(GDT_KERNEL_CODE, coproc_segment_overrun_handler_entry);
    idt.invalid_tss              = DEFINE_IDT64_ENTRY_INTERRUPT(GDT_KERNEL_CODE, invalid_tss_handler_entry);
    idt.segment_not_present      = DEFINE_IDT64_ENTRY_INTERRUPT(GDT_KERNEL_CODE, segment_not_present_handler_entry);
    idt.stack_segment_fault      = DEFINE_IDT64_ENTRY_INTERRUPT(GDT_KERNEL_CODE, stack_segment_fault_handler_entry);
    idt.general_protection_fault = DEFINE_IDT64_ENTRY_INTERRUPT(GDT_KERNEL_CODE, general_protection_fault_handler_entry);
    idt.page_fault               = DEFINE_IDT64_ENTRY_INTERRUPT(GDT_KERNEL_CODE, page_fault_handler_entry);
    idt.x87_fpu_error            = DEFINE_IDT64_ENTRY_INTERRUPT(GDT_KERNEL_CODE, x87_fpu_handler_entry);
    idt.alignment_check          = DEFINE_IDT64_ENTRY_INTERRUPT(GDT_KERNEL_CODE, alignment_check_handler_entry);
    idt.machine_check            = DEFINE_IDT64_ENTRY_INTERRUPT(GDT_KERNEL_CODE, machine_check_handler_entry);
    idt.simd_fp_error            = DEFINE_IDT64_ENTRY_INTERRUPT(GDT_KERNEL_CODE, simd_fpu_error_handler_entry);

    idt._res0 = DEFINE_IDT64_ENTRY_INTERRUPT(GDT_KERNEL_CODE, null_handler_entry);
    idt._res1 = DEFINE_IDT64_ENTRY_INTERRUPT(GDT_KERNEL_CODE, null_handler_entry);
    idt._res2 = DEFINE_IDT64_ENTRY_INTERRUPT(GDT_KERNEL_CODE, null_handler_entry);

    for (uint16_t i = 0; i < 8; i++) idt.mapped_irqs[i] = DEFINE_IDT64_ENTRY_INTERRUPT(GDT_KERNEL_CODE, null_pic1_handler_entry);
    for (uint16_t i = 8; i < 16; i++) idt.mapped_irqs[i] = DEFINE_IDT64_ENTRY_INTERRUPT(GDT_KERNEL_CODE, null_pic2_handler_entry);

    // idt.system_interrupt = DEFINE_IDT64_ENTRY_USER_INTERRUPT(GDT_KERNEL_CODE, syscall_handler_entry);

    interrupt_pic_mask_all();

    if (false && interrupt_apic_available()) { // TODO: figure out the APIC
        interrupt_apic_init();
    }
    else {
        interrupt_pic_init();
    }

    lidt(&idt_ptr);
}