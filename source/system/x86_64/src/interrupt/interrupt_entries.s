.code64

.macro EXCEPTION_INTERRUPT name, handler
.global \name
\name:
    pushf
    push 8(%rsp)
    push %r15
    push %r14
    push %r13
    push %r12
    push %r11
    push %r10
    push %r9
    push %r8
    push 104(%rsp)
    push %rbp
    push %rdi
    push %rsi
    push %rdx
    push %rcx
    push %rbx
    push %rax

    movq %rsp, %rdi

    cld
    call \handler
.endm

.macro PIC1_INTERRUPT name, handler
.global \name
\name:
    pushf
    push 8(%rsp)
    push %r15
    push %r14
    push %r13
    push %r12
    push %r11
    push %r10
    push %r9
    push %r8
    push 104(%rsp)
    push %rbp
    push %rdi
    push %rsi
    push %rdx
    push %rcx
    push %rbx
    push %rax

    movq %rsp, %rdi

    mov  $0x20, %al
    outb %al, $0x20

    cld
    call \handler
.endm

.macro PIC2_INTERRUPT name, handler
.global \name
\name:
    pushf
    push 8(%rsp)
    push %r15
    push %r14
    push %r13
    push %r12
    push %r11
    push %r10
    push %r9
    push %r8
    push 104(%rsp)
    push %rbp
    push %rdi
    push %rsi
    push %rdx
    push %rcx
    push %rbx
    push %rax

    movq %rsp, %rdi

    mov  $0x20, %al
    outb %al, $0x20
    outb %al, $0xA0

    cld
    call \handler
.endm

EXCEPTION_INTERRUPT null_handler_entry, null_handler

EXCEPTION_INTERRUPT div0_handler_entry, div0_handler
EXCEPTION_INTERRUPT nmi_handler_entry, nmi_handler
EXCEPTION_INTERRUPT bp_int3_handler_entry, bp_int3_handler
EXCEPTION_INTERRUPT ovf_handler_entry, ovf_handler
EXCEPTION_INTERRUPT bound_range_handler_entry, bound_range_handler
EXCEPTION_INTERRUPT invalid_opcode_handler_entry, invalid_opcode_handler
EXCEPTION_INTERRUPT device_not_avail_handler_entry, device_not_avail_handler
EXCEPTION_INTERRUPT double_fault_handler_entry, double_fault_handler
EXCEPTION_INTERRUPT coproc_segment_overrun_handler_entry, coproc_segment_overrun_handler
EXCEPTION_INTERRUPT invalid_tss_handler_entry, invalid_tss_handler
EXCEPTION_INTERRUPT segment_not_present_handler_entry, segment_not_present_handler
EXCEPTION_INTERRUPT stack_segment_fault_handler_entry, stack_segment_fault_handler
EXCEPTION_INTERRUPT general_protection_fault_handler_entry, general_protection_fault_handler
EXCEPTION_INTERRUPT page_fault_handler_entry, page_fault_handler
EXCEPTION_INTERRUPT x87_fpu_handler_entry, x87_fpu_handler
EXCEPTION_INTERRUPT alignment_check_handler_entry, alignment_check_handler
EXCEPTION_INTERRUPT machine_check_handler_entry, machine_check_handler
EXCEPTION_INTERRUPT simd_fpu_error_handler_entry, simd_fpu_error_handler

PIC1_INTERRUPT null_pic1_handler_entry, null_handler
PIC1_INTERRUPT pic1_keyboard_handler_entry, pic1_keyboard_handler
PIC1_INTERRUPT pic1_timer_handler_entry, pic1_timer_handler

PIC2_INTERRUPT null_pic2_handler_entry, null_handler
