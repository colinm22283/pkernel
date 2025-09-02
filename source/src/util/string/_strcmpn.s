.code64

.global strcmpn
strcmpn:
    # rdi: str a
    # rsi: str b
    # rdx: length

    xor %rax, %rax

    mov %rdx, %rcx

    .loop:
        movb (%rdi), %al
        movb (%rsi), %dl

        sub  %dl,    %al
        jnz  ..break

        loop .loop
    ..break:

    movsx %al, %rax
    ret