.code64

.section .kernel_entry, "a"

.global kernel_entry
kernel_entry:
    movabs $_stack_top, %rsp
    mov    %rsp,        %rbp

    cld
    jmp kernel_main

