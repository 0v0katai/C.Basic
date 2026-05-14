#include <limits.h>
#include <stdbool.h>

#include "CB.h"

void StrDMSsub(char *buffer, double a) {
    int degree, minute;
    double second;

    if (a > INT_MAX) a = INT_MAX;
    else if (a < INT_MIN) a = INT_MIN;
    bool minus = a < 0;
    a = fabs(a);

    degree = (int)a;
    minute = (int)((a - degree) * 60.);
    second = ((a - degree) * 60. - minute) * 60.;

    int len = sprintf(buffer, "%s%d\x9C%02d\xE5\x96%05.2f",
        minus ? "\x87" : "", degree, minute, second);
    char *p = &buffer[len];
    if (p[-1] == '0') p--;
    if (p[-1] == '0') p--;
    if (p[-1] == '.') p--;
    p[0] = '\xE5';
    p[1] = '\x98';
    p[2] = '\0';
}
