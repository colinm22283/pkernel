#pragma once

#ifdef DEBUG_LOGGER_ENABLED

#define DEFINE_DEBUG_LOGGER(str) static const char _debug_logger_name[] = str

#define DEBUG_LOG(...) do { debug_print("[KERNEL "); debug_print(_debug_logger_name); debug_print(" ] "); __VA_ARGS__; debug_print("\n"); } while (0);
#define DEBUG_PRINT(str) debug_print(str);
#define DEBUG_PRINT_HEX(num) debug_print_hex(num);

#else

#define DEFINE_DEBUG_LOGGER(str)

#define DEBUG_LOG(...)
#define DEBUG_PRINT(str)
#define DEBUG_PRINT_HEX(num)

#endif
