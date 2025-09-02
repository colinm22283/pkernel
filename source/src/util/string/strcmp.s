.code64

.global strcmp
strcmp:
    # rdi: string a
    # rsi: string b

    xor  %rax, %rax

    .loop:
        movb (%rdi), %cl
        movb (%rsi), %ch

        mov  %cl,    %al
        sub  %ch,    %al
        jnz  ..break

        test %cl,    %cl
        jz   ..break

        inc %rdi
        inc %rsi

        test %ch,    %ch
        jnz .loop
    ..break:

    movsx %al, %rax
    ret
