#include <util/string/strcmpn.h>

int64_t strcmpn(const char * a, const char * b, uint64_t length) {
    for (uint64_t i = 0; i < length; i++) {
        if (*a != *b) {
            return a - b;
        }

        if (*a == '\0' || *b == '\0') break;

        a++;
        b++;
    }

    return 0;
}