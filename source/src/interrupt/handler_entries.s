.code64

.global null_handler_entry
null_handler_entry:
    hlt

    iretq

.global div0_handler_entry
div0_handler_entry:
    push %rax
    push %rcx
    push %rdx
    push %rsi
    push %rdi
    push %r8
    push %r9
    push %r10
    push %r11

    cld
    call div0_handler

    pop %r11
    pop %r10
    pop %r9
    pop %r8
    pop %rdi
    pop %rsi
    pop %rdx
    pop %rcx
    pop %rax

    iretq

.global nmi_handler_entry
nmi_handler_entry:
    push %rax
    push %rcx
    push %rdx
    push %rsi
    push %rdi
    push %r8
    push %r9
    push %r10
    push %r11

    cld
    call nmi_handler

    pop %r11
    pop %r10
    pop %r9
    pop %r8
    pop %rdi
    pop %rsi
    pop %rdx
    pop %rcx
    pop %rax

    iretq


.global bp_int3_handler_entry
bp_int3_handler_entry:
    push %rax
    push %rcx
    push %rdx
    push %rsi
    push %rdi
    push %r8
    push %r9
    push %r10
    push %r11

    cld
    call bp_int3_handler

    pop %r11
    pop %r10
    pop %r9
    pop %r8
    pop %rdi
    pop %rsi
    pop %rdx
    pop %rcx
    pop %rax

    iretq


.global ovf_handler_entry
ovf_handler_entry:
    push %rax
    push %rcx
    push %rdx
    push %rsi
    push %rdi
    push %r8
    push %r9
    push %r10
    push %r11

    cld
    call ovf_handler

    pop %r11
    pop %r10
    pop %r9
    pop %r8
    pop %rdi
    pop %rsi
    pop %rdx
    pop %rcx
    pop %rax

    iretq


.global bound_range_handler_entry
bound_range_handler_entry:
    push %rax
    push %rcx
    push %rdx
    push %rsi
    push %rdi
    push %r8
    push %r9
    push %r10
    push %r11

    cld
    call bound_range_handler

    pop %r11
    pop %r10
    pop %r9
    pop %r8
    pop %rdi
    pop %rsi
    pop %rdx
    pop %rcx
    pop %rax

    iretq


.global invalid_opcode_handler_entry
invalid_opcode_handler_entry:
    push %rax
    push %rcx
    push %rdx
    push %rsi
    push %rdi
    push %r8
    push %r9
    push %r10
    push %r11

    cld
    call invalid_opcode_handler

    pop %r11
    pop %r10
    pop %r9
    pop %r8
    pop %rdi
    pop %rsi
    pop %rdx
    pop %rcx
    pop %rax

    iretq


.global device_not_avail_handler_entry
device_not_avail_handler_entry:
    push %rax
    push %rcx
    push %rdx
    push %rsi
    push %rdi
    push %r8
    push %r9
    push %r10
    push %r11

    cld
    call device_not_avail_handler

    pop %r11
    pop %r10
    pop %r9
    pop %r8
    pop %rdi
    pop %rsi
    pop %rdx
    pop %rcx
    pop %rax

    iretq


.global double_fault_handler_entry
double_fault_handler_entry:
    cli
    hlt


.global coproc_segment_overrun_handler_entry
coproc_segment_overrun_handler_entry:
    push %rax
    push %rcx
    push %rdx
    push %rsi
    push %rdi
    push %r8
    push %r9
    push %r10
    push %r11

    cld
    call coproc_segment_overrun_handler

    pop %r11
    pop %r10
    pop %r9
    pop %r8
    pop %rdi
    pop %rsi
    pop %rdx
    pop %rcx
    pop %rax

    iretq


.global invalid_tss_handler_entry
invalid_tss_handler_entry:
    push %rax
    push %rcx
    push %rdx
    push %rsi
    push %rdi
    push %r8
    push %r9
    push %r10
    push %r11

    cld
    call invalid_tss_handler

    pop %r11
    pop %r10
    pop %r9
    pop %r8
    pop %rdi
    pop %rsi
    pop %rdx
    pop %rcx
    pop %rax

    iretq


.global segment_not_present_handler_entry
segment_not_present_handler_entry:
    push %rax
    push %rcx
    push %rdx
    push %rsi
    push %rdi
    push %r8
    push %r9
    push %r10
    push %r11

    cld
    call segment_not_present_handler

    pop %r11
    pop %r10
    pop %r9
    pop %r8
    pop %rdi
    pop %rsi
    pop %rdx
    pop %rcx
    pop %rax

    iretq


.global stack_segment_fault_handler_entry
stack_segment_fault_handler_entry:
    push %rax
    push %rcx
    push %rdx
    push %rsi
    push %rdi
    push %r8
    push %r9
    push %r10
    push %r11

    cld
    call stack_segment_fault_handler

    pop %r11
    pop %r10
    pop %r9
    pop %r8
    pop %rdi
    pop %rsi
    pop %rdx
    pop %rcx
    pop %rax

    iretq


.global general_protection_fault_handler_entry
general_protection_fault_handler_entry:
    hlt

    push %rax
    push %rcx
    push %rdx
    push %rsi
    push %rdi
    push %r8
    push %r9
    push %r10
    push %r11

    cld
    call general_protection_fault_handler

    pop %r11
    pop %r10
    pop %r9
    pop %r8
    pop %rdi
    pop %rsi
    pop %rdx
    pop %rcx
    pop %rax

    iretq


.global page_fault_handler_entry
page_fault_handler_entry:
    pushf
    push 16(%rsp)
    push %r15
    push %r14
    push %r13
    push %r12
    push %r11
    push %r10
    push %r9
    push %r8
    mov 112(%rsp), %r8
    push %r8
    push %rbp
    push %rdi
    push %rsi
    push %rdx
    push %rcx
    push %rbx
    push %rax

    movq %rsp, %rdi

    mov 144(%rsp), %rsi

    cld
    call page_fault_handler

    add $152, %rsp

    iretq


.global x87_fpu_handler_entry
x87_fpu_handler_entry:
    push %rax
    push %rcx
    push %rdx
    push %rsi
    push %rdi
    push %r8
    push %r9
    push %r10
    push %r11

    cld
    call x87_fpu_handler

    pop %r11
    pop %r10
    pop %r9
    pop %r8
    pop %rdi
    pop %rsi
    pop %rdx
    pop %rcx
    pop %rax

    iretq


.global alignment_check_handler_entry
alignment_check_handler_entry:
    push %rax
    push %rcx
    push %rdx
    push %rsi
    push %rdi
    push %r8
    push %r9
    push %r10
    push %r11

    cld
    call alignment_check_handler

    pop %r11
    pop %r10
    pop %r9
    pop %r8
    pop %rdi
    pop %rsi
    pop %rdx
    pop %rcx
    pop %rax

    iretq


.global machine_check_handler_entry
machine_check_handler_entry:
    push %rax
    push %rcx
    push %rdx
    push %rsi
    push %rdi
    push %r8
    push %r9
    push %r10
    push %r11

    cld
    call machine_check_handler

    pop %r11
    pop %r10
    pop %r9
    pop %r8
    pop %rdi
    pop %rsi
    pop %rdx
    pop %rcx
    pop %rax

    iretq


.global simd_fpu_error_handler_entry
simd_fpu_error_handler_entry:
    push %rax
    push %rcx
    push %rdx
    push %rsi
    push %rdi
    push %r8
    push %r9
    push %r10
    push %r11

    cld
    call simd_fpu_error_handler

    pop %r11
    pop %r10
    pop %r9
    pop %r8
    pop %rdi
    pop %rsi
    pop %rdx
    pop %rcx
    pop %rax

    iretq

.global null_pic1_handler
null_pic1_handler:
    push %rax

    mov  $0x20, %al
    outb %al, $0x20

    pop %rax

    iretq


.global null_pic2_handler
null_pic2_handler:
    push %rax

    mov  $0x20, %al
    outb %al, $0x20
    outb %al, $0xA0

    pop %rax

    iretq

.global pic1_keyboard_handler_entry
pic1_keyboard_handler_entry:
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
    call pic1_keyboard_handler

.global pic1_timer_handler_entry
pic1_timer_handler_entry:
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
    call pic1_timer_handler
