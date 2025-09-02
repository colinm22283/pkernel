.code64

.global memcpy
memcpy:
    mov  %rdx,     %rcx

    test %rcx,     %rcx
    jz   .ret

    .loop:
        movb (%rsi), %dl
        movb %dl,    (%rdi)

        inc  %rsi
        inc  %rdi

        loop .loop

.ret:
    ret
