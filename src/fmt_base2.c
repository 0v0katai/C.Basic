#include "CB.h"

int CB_Hex(char *SRC) {
    CB_CurrentStr = NewStrBuffer();
    if (ErrorNo) return 0;

    sprintf(CB_CurrentStr, "%X", CB_EvalInt(SRC));
    if (SRC[ExecPtr] == ')') ExecPtr++;
    return CB_StrBufferMax-1;
}

int CB_Bin(char *SRC) {
    CB_CurrentStr = NewStrBuffer();
    if (ErrorNo) return 0;

    sprintf(CB_CurrentStr, "%b", CB_EvalInt(SRC));
    if (SRC[ExecPtr] == ')') ExecPtr++;
    return CB_StrBufferMax-1;
}
