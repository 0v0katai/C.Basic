#ifndef MINMAX_H
#define MINMAX_H

#ifdef __cplusplus
extern "C" {
#endif

#define min(x, y) ({ \
    __typeof__ (x) _x = (x); \
    __typeof__ (y) _y = (y); \
    (_x < _y) ? (_x) : (_y); \
})

#define max(x, y) ({ \
    __typeof__ (x) _x = (x); \
    __typeof__ (y) _y = (y); \
    (_x > _y) ? (_x) : (_y); \
})

int va_min_int(int count, int first, ...);
int va_max_int(int count, int first, ...);
double va_min_double(int count, double first, ...);
double va_max_double(int count, double first, ...);

#define va_min(count, first, ...) \
    _Generic((first), double : va_min_double, \
                      float  : va_min_double, \
                      default: va_min_int)(count, first, __VA_ARGS__)

#define va_max(count, first, ...) \
    _Generic((first), double : va_max_double, \
                      float  : va_max_double, \
                      default: va_max_int)(count, first, __VA_ARGS__)

#ifdef __cplusplus
}
#endif

#endif /* MINMAX_H */