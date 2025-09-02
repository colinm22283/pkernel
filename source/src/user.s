.code64

OPEN = 0
WRITE = 1
READ = 2
SEEK = 3
EXIT = 4

user_data = 0x8200000000

.section .text
.global user_entry
user_entry:
    mov    $OPEN, %rax
    movabs $(disc_path + user_data - user_data_start), %rsi
    mov    $0b11, %rdx
    int    $0x30
    push   %rax

    mov    $OPEN,        %rax
    movabs $(tty_path + user_data - user_data_start),    %rsi
    mov    $0b10,        %rdx
    int    $0x30
    push   %rax

    mov    $SEEK,       %rax
    mov    8(%rsp),     %rsi
    mov    $(64 * 512), %rdx
    mov    $0,          %rcx
    int    $0x30

    mov    $READ,   %rax
    mov    8(%rsp), %rsi
    movabs $(user_buffer + user_data - user_data_start), %rdx
    mov    $10,     %rcx
    int    $0x30

    mov    $WRITE,  %rax
    mov    0(%rsp), %rsi
    movabs $(user_buffer + user_data - user_data_start), %rdx
    mov    $10,     %rcx
    int    $0x30

    #mov    $EXIT,        %rax
    #mov    $0,           %rsi
    #int    $0x30

    mov    $OPEN,        %rax
    movabs $(kbd_path + user_data - user_data_start),    %rsi
    mov    $0b01,        %rdx
    int    $0x30
    push   %rax

    mov    $WRITE,       %rax
    mov    8(%rsp),     %rsi
    movabs $(user_string + user_data - user_data_start), %rdx
    mov    $12,          %rcx
    int    $0x30

    .loop:
        mov    $READ,        %rax
        mov    0(%rsp),      %rsi
        movabs $(user_buffer + user_data - user_data_start), %rdx
        mov    $1,           %rcx
        int    $0x30

        cmp    $0,           %rax
        jl     .error
        je     .loop

        mov    $WRITE,       %rax
        mov    8(%rsp),     %rsi
        movabs $(user_buffer + user_data - user_data_start), %rdx
        mov    $1,           %rcx
        int    $0x30

        jmp .loop

    mov    $EXIT,        %rax
    mov    $0,           %rsi
    int    $0x30

    .error:
        mov    $WRITE,       %rax
        mov    8(%rsp),      %rsi
        movabs $(error_string + user_data - user_data_start), %rdx
        mov    $6,           %rcx
        int    $0x30

        jmp .loop

.section .rodata, "a"
.global user_data_start
user_data_start:

.global user_string
user_string: .string "HELLO WORLD\n"

.global error_string
error_string: .string "ERROR\n"

.global tty_path
tty_path: .asciz "dev/tty"

.global kbd_path
kbd_path: .asciz "dev/kbd"

.global kbd_path
disc_path: .asciz "dev/disc0"

.global user_buffer
user_buffer: .byte 'A'
