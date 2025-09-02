#include <util/memory/memcpy.h>

void memcpy(void * _dst, const void * _src, uint64_t size) {
    char * dst = _dst;
    const char * src = _src;

    for (uint64_t i = 0; i < size; i++) dst[i] = src[i];
}