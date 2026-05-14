#include "minmax.h"

double min_dbl(int count, const double arr[]) {
    double m = arr[0];
    for (int i = 1; i < count; i++)
        if (arr[i] < m) m = arr[i];
    return m;
}

double max_dbl(int count, const double arr[]) {
    double m = arr[0];
    for (int i = 1; i < count; i++)
        if (arr[i] > m) m = arr[i];
    return m;
}

int min_int(int count, const int arr[]) {
    int m = arr[0];
    for (int i = 1; i < count; i++)
        if (arr[i] < m) m = arr[i];
    return m;
}

int max_int(int count, const int arr[]) {
    int m = arr[0];
    for (int i = 1; i < count; i++)
        if (arr[i] > m) m = arr[i];
    return m;
}
