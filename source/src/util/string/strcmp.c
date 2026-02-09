#include <stddef.h>
#include <stdbool.h>

#include <util/string/strcmp.h>

#include <sys/debug/print.h>

int64_t strcmp(const char * a, const char * b) {
    for (size_t i = 0; true; i++) {
        if (a[i] != b[i]) return a[i] - b[i];

        if (a[i] == '\0') return 0;
    }
}