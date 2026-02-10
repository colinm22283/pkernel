.code64

SYSCALL_SIGNALRET = 25

.global process_trampoline
process_trampoline:
    

    int $SYSCALL_SIGNALRET

