#include <string.h>
#include <stdio.h>
#include <ctype.h>

#include "CB.h"
#include "syntax.h"

int CB_Sprintf(char *SRC) {
    int maxoplen __attribute__((unused));
    char *fmt = CB_GetOpStr(SRC, &maxoplen);
    if (ErrorNo) return 0;

    char buffer[256] = {0};
    int idx = 0;

    for (char fmt_c; (fmt_c = *fmt); fmt++) {
        if (fmt_c != '%') {
            if (idx < sizeof(buffer) - 1) buffer[idx++] = fmt_c;
            continue;
        }
        char fmt_part[256] = "%";
        for (int i = 1; i < sizeof(fmt_part) - 1; i++) {
            fmt_c = *++fmt;
            if (fmt_c == '*') {
                syntax_check_comma(0);
                syntax_skip('%');
                i += snprintf(fmt_part + i, sizeof(fmt_part) - i, "%d", CB_EvalInt(SRC)) - 1;
                continue;
            }
            fmt_part[i] = fmt_c;
            if (fmt_c == '%') break;
            if (strchr("hljztL", fmt_c)) continue;
            if (tolower(fmt_c) >= 'a' && tolower(fmt_c) <= 'z') break;
        }
        if (fmt_c == '%') {
            if (idx < sizeof(buffer) - 1) buffer[idx++] = '%';
            continue;
        }
        syntax_check_comma(0);

        int n = -1;
        if (fmt_c == 's') {
            if (CB_IsStr(SRC, ExecPtr))
                n = snprintf(&buffer[idx], sizeof(buffer) - idx, fmt_part, CB_GetOpStr(SRC, &maxoplen));
        } else if (strchr("aAeEfFgG", fmt_c)) {
            if (CB_INT == 0) syntax_skip('#');
            n = snprintf(&buffer[idx], sizeof(buffer) - idx, fmt_part, CB_EvalDbl(SRC));
        } else {
            syntax_skip('%');
            n = snprintf(&buffer[idx], sizeof(buffer) - idx, fmt_part, CB_EvalInt(SRC));
        }
        if (n < 0) {
            CB_Error(ArgumentERR);
            return 0;
        }
        if (n >= (int)(sizeof(buffer) - idx)) {
            CB_Error(StringTooLong);
            return 0;
        }
        idx += n;
    }
    syntax_skip(')');

    CB_CurrentStr = NewStrBuffer();
    if (ErrorNo) return 0;
    memcpy(CB_CurrentStr, buffer, sizeof(buffer));

    for (char *p = CB_CurrentStr; *p; p++) {
        switch (*p) {
            case '-':
                *p = 0x87;
                break;
            case 'E':
            case 'e':
                *p = 0x0F;
                break;
        }
    }

    return CB_StrBufferMax-1;
}
