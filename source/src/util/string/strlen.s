.code64

.global strlen
strlen:
    # rdi: string

    xor  %rax,   %rax

    movb (%rdi), %cl

    .loop:
        test %cl,    %cl
        jz   .return

        inc  %rax
        inc  %rdi

        movb (%rdi), %cl
        jmp  .loop

.return:
    ret
