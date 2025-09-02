.code64

.global syscall_handler_entry
syscall_handler_entry:
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
    mov 104(%rsp), %r8
    push %r8
    push %rbp
    push %rdi
    push %rsi
    push %rdx
    push %rcx
    push %rbx
    push %rax

    movq %rsp, %r9

    mov %rax, %rdi

    cld
    call syscall_handler

    pop %r11
    pop %r10
    pop %r9
    pop %r8
    pop %rdi
    pop %rsi
    pop %rdx
    pop %rcx

    add $144, %rsp

    iretq