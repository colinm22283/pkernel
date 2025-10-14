.code64

.global _resume_tsr_kernel
_resume_tsr_kernel: # rdi: CS selector, rsi: SS selector, rdx: pml4t paddr, rcx: tsr
    cli

    and   $(~0x0000000000000003), %rdi
    and   $(~0x0000000000000003), %rsi

    mov   %si,   %ds
    mov   %si,   %es
    mov   %si,   %fs
    mov   %si,   %gs

    mov  56(%rcx),  %r8
    mov  128(%rcx), %r9

    mov  %r8,  %rsp

    mov  %rdx, %cr3

    push  %rsi # SS selection
    push  %r8  # stack pointer

    # flags
    pushf
    mov   $(~0b100000000), %rax
    and   %rax, (%rsp)

    push  %rdi # CS selector
    push  %r9  # instruction pointer

    mov   %rcx, %r15

    mov   120(%r15), %rax
    mov   %rax,      -16(%rsp)

    mov   0(%r15), %rax
    mov   8(%r15), %rbx
    mov   16(%r15), %rcx
    mov   24(%r15), %rdx
    mov   32(%r15), %rsi
    mov   40(%r15), %rdi
    mov   48(%r15), %rbp
    # rsp
    mov   64(%r15), %r8
    mov   72(%r15), %r9
    mov   80(%r15), %r10
    mov   88(%r15), %r11
    mov   96(%r15), %r12
    mov   104(%r15), %r13
    mov   112(%r15), %r14
    mov   120(%r15), %r15

    iretq
