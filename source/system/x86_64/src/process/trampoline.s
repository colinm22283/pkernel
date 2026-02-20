.code64

SYSCALL_SIGNALRET = 25

.global process_trampoline
process_trampoline:
    mov $SYSCALL_SIGNALRET, %rax
    int $0x30
process_trampoline_end:

.global process_trampoline_size
process_trampoline_size: .quad (process_trampoline_end - process_trampoline)
