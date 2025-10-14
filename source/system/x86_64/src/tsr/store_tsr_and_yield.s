.code64

.global _store_tsr_and_yield
_store_tsr_and_yield: # rdi: tsr
    mov %rax,   0(%rdi)
    mov %rbx,   8(%rdi)
    mov %rcx,  16(%rdi)
    mov %rdx,  24(%rdi)
    mov %rdi,  32(%rdi)
    mov %rsi,  40(%rdi)
    mov %rbp,  48(%rdi)
    mov %rsp,  56(%rdi)
    mov %r8,   64(%rdi)
    mov %r9,   72(%rdi)
    mov %r10,  80(%rdi)
    mov %r11,  88(%rdi)
    mov %r12,  96(%rdi)
    mov %r13, 104(%rdi)
    mov %r14, 112(%rdi)
    mov %r15, 120(%rdi)

    pushf
    mov (%rsp), %rax
    mov %rax, 136(%rdi)
    add $8, %rsp

    mov $_store_tsr_and_yield_return, 128(%rdi)

    jmp scheduler_yield

_store_tsr_and_yield_return:
    ret
