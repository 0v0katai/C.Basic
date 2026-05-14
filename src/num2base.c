#include <stdio.h>
#include <string.h>
#include <stdint.h>

void NumToBin( char *buffer, int n, int digit) {
    sprintf(buffer, "%0*b", digit, n);
}

void NumToHex( char *buffer, int n, int digit) {
    sprintf(buffer, "%0*X", digit, n & 0xFFFFFFFF >> (32 - (digit << 2)));
}

void DNumToHex(char *buffer, double x) {
    uint64_t hex;
    memcpy(&hex, &x, sizeof(double));
    sprintf(buffer, "%016llX", hex);
}
