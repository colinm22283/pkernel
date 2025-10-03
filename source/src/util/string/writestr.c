#include <util/string/writestr.h>

uint64_t writestr(char * buf, uint64_t size, uint64_t offset, uint64_t value) {
    char buffer[20];

    uint64_t num = value;
    for (uint64_t i = 0; i < 20; i++) {
        buffer[19 - i] = (char) ('0' + (num % 10));
        num /= 10;
    }

    uint64_t base = 0;
    uint64_t real_size = 20;
    for (uint64_t i = 0; i < 19; i++) {
        if (buffer[i] != '0') break;
        else {
            base++;
            real_size--;
        }
    }

    if (offset >= real_size) return 0;
    if (size + offset >= real_size) size = real_size - offset;

    for (uint64_t i = 0; i < size; i++) {
        buf[i] = buffer[base + offset + i];
    }

    return size;
}
