#ifndef MINMAX_H
#define MINMAX_H

#ifdef __cplusplus
extern "C" {
#endif

#define GET_FIRST(first, ...) first

#define OP(op, ...) _Generic((GET_FIRST(__VA_ARGS__)), \
    double:  op##_dbl(sizeof((double[]){__VA_ARGS__}) / sizeof(double), (double[]){__VA_ARGS__}), \
    float:   op##_dbl(sizeof((double[]){__VA_ARGS__}) / sizeof(double), (double[]){__VA_ARGS__}), \
    default: op##_int(sizeof((int[]){__VA_ARGS__}) / sizeof(int), (int[]){__VA_ARGS__}) \
)

#define min(...) OP(min, __VA_ARGS__)
#define max(...) OP(max, __VA_ARGS__)

double min_dbl(int count, const double arr[]);
double max_dbl(int count, const double arr[]);
int min_int(int count, const int arr[]);
int max_int(int count, const int arr[]);

#ifdef __cplusplus
}
#endif

#endif /* MINMAX_H */
