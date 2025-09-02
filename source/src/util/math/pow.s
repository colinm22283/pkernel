.code64

.global math_pow
math_pow:
    mov $1,   %rax
    xor %rdx, %rdx

    .loop:
        imul %rdi

        dec  %rsi

        cmp  $0, %rsi
        jne  .loop

    ret