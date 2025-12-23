#include <stdarg.h>
#include "va_lib.h"

int min(int count, ...) {
    va_list args;
    va_start(args, count);

    int min = va_arg(args, int);
    for (int i = 0; i < count-1; ++i) {
        int next = va_arg(args, int);
        min = min < next ? min : next;
    }
    va_end(args);

    return min;
}

int max(int count, ...) {
    va_list args;
    va_start(args, count);

    int max = va_arg(args, int);
    for (int i = 0; i < count-1; ++i) {
        int next = va_arg(args, int);
        max = max > next ? max : next;
    }
    va_end(args);

    return max;
}
