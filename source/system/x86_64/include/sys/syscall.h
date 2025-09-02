#pragma once

#include <sys/interrupt.h>

#define _syscall_reg_arg2(arg, ...)                         \
    register uint64_t ___arg2 asm ("rcx") = (uint64_t) arg;

#define _syscall_reg_arg1(arg, ...)                         \
    register uint64_t ___arg1 asm ("rdx") = (uint64_t) arg; \
    __VA_OPT__(_syscall_reg_arg2(__VA_ARGS__));

#define _syscall_reg_arg0(arg, ...)                         \
    register uint64_t ___arg0 asm ("rsi") = (uint64_t) arg; \
    __VA_OPT__(_syscall_reg_arg1(__VA_ARGS__));

#define _syscall_asm_arg2(arg, ...) "r" (___arg2)
#define _syscall_asm_arg1(arg, ...) "r" (___arg1) __VA_OPT__(, _syscall_asm_arg2(__VA_ARGS__))
#define _syscall_asm_arg0(arg, ...) "r" (___arg0) __VA_OPT__(, _syscall_asm_arg1(__VA_ARGS__))

#define syscall(sel, ...) do {                                    \
    register uint64_t ___a asm ("rdi") = sel;                     \
    __VA_OPT__(_syscall_reg_arg0(__VA_ARGS__));                   \
    asm(                                                          \
        "int $0x80" : :                                           \
        "r" (___a) __VA_OPT__(, _syscall_asm_arg0(__VA_ARGS__)) : \
        "rax"                                                     \
    );                                                            \
} while (0)

#define syscall_ret(sel, ...) ({                                    \
    register uint64_t ___b asm ("rax");                             \
    do {                                                            \
        register uint64_t ___a asm ("rdi") = sel;                   \
        __VA_OPT__(_syscall_reg_arg0(__VA_ARGS__));                 \
        asm volatile (                                              \
            "int $0x80" :                                           \
            "=r" (___b) :                                           \
            "r" (___a) __VA_OPT__(, _syscall_asm_arg0(__VA_ARGS__)) \
        );                                                          \
    } while (0);                                                    \
    ___b;                                                           \
})
