.code64

.global memset
memset:
    mov %rdx, %rcx
    mov %rsi, %rdx

    test %rcx,     %rcx
    jz   .ret

    .loop:
        movb %dl, (%rdi)
        inc  %rdi

        loop .loop

.ret:
    ret
