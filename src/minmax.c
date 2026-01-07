#include <stdarg.h>
#include "minmax.h"

int va_min_int(int count, int first, ...) {
    va_list args;
    va_start(args, first);

    int min = first;
    for (int i = 0; i < count-1; ++i) {
        int next = va_arg(args, int);
        min = min < next ? min : next;
    }
    va_end(args);

    return min;
}

int va_max_int(int count, int first, ...) {
    va_list args;
    va_start(args, first);

    int max = first;
    for (int i = 0; i < count-1; ++i) {
        int next = va_arg(args, int);
        max = max > next ? max : next;
    }
    va_end(args);

    return max;
}

double va_min_double(int count, double first, ...) {
    va_list args;
    va_start(args, first);

    double min = first;
    for (int i = 0; i < count-1; ++i) {
        double next = va_arg(args, double);
        min = min < next ? min : next;
    }
    va_end(args);

    return min;
}

double va_max_double(int count, double first, ...) {
    va_list args;
    va_start(args, first);

    double max = first;
    for (int i = 0; i < count-1; ++i) {
        double next = va_arg(args, double);
        max = max > next ? max : next;
    }
    va_end(args);

    return max;
}