.code64

.global strcpy
strcpy:
    # rdi: dst
    # rsi: src

    .loop:
        movb (%rsi), %al
        movb %al,    (%rdi)

        inc  %rdi
        inc  %rsi

        test %al,    %al
        jnz  .loop

    ret
