#pragma once

#define __STDCALL    __attribute__((stdcall))
#define __VECTORCALL __attribute__((vectorcall))
#define __SYSV_ABI   __attribute__((sysv_abi))
#define __FASTCALL   __attribute__((fastcall))
#define __CDECL      __attribute__((cdecl))

#define __PACKED               __attribute__((packed))
#define __NORETURN             __attribute__((noreturn))
#define __UNUSED               __attribute__((unused))
#define __INTERRUPT            __attribute__((interrupt))
#define __INTERRUPT_SUBROUTINE __attribute__((no_caller_saved_registers))
#define __NONNULL               __attribute__((nonnull))
#define __UNUSED               __attribute__((unused))
#define __NOINLINE             __attribute__((noinline))
#define __ALWAYS_INLINE        __attribute__((always_inline))
#define __MAYBE_UNUSED         __attribute__((unused))
#define __NONSTRING            __attribute__((nonstring))

#define __UNREACHABLE()        __builtin_unreachable()

#define __SECTION(section_name) __attribute__((__section__(section_name)))
#define __ALIGNED(alignment) __attribute__((aligned(alignment)))

#define GDT_CODE_SEG (0x08)
#define GDT_DATA_SEG (0x10)
