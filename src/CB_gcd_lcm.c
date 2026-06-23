#include "CB.h"
#include "syntax.h"

#define GCD_LCM_PARSER_BODY(TYPE, LIST_EVAL, SCALAR_EVAL, MATRIX_READ, OP, ERROR_RET) \
    int start = ExecPtr; \
    TYPE result = SCALAR_EVAL(SRC); \
    if (CB_MatListAnsreg >= 28) CB_MatListAnsreg = 28; \
    if (dspflag >= 3) { \
        if (dspflag != 4) { \
            CB_Error(ArgumentERR); \
            return (ERROR_RET); \
        } \
        ExecPtr = start; \
        LIST_EVAL(SRC); \
        int list_idx = CB_MatListAnsreg; \
        int size = MatAry[list_idx].SizeA; \
        int base = MatAry[list_idx].Base; \
        for (int ptr = base; ptr < size + base; ptr++) \
            result = OP(result, MATRIX_READ(list_idx, ptr, base)); \
        dspflag = 2; \
    } else { \
        while (SRC[ExecPtr] == ',') { \
            ExecPtr++; \
            result = OP(result, SCALAR_EVAL(SRC)); \
        } \
    } \
    syntax_skip(')'); \
    return result;

static complex parser_cplx(char *SRC, complex (*op)(complex, complex)) {
    GCD_LCM_PARSER_BODY(complex, Cplx_ListEvalsubTop, Cplx_EvalsubTop, Cplx_ReadMatrix, op, Int2Cplx(0))
}

static double parser_dbl(char *SRC, double (*op)(double, double)) {
    GCD_LCM_PARSER_BODY(double, ListEvalsubTop, EvalsubTop, ReadMatrix, op, 0.0)
}

static int parser_int(char *SRC, int (*op)(int, int)) {
    GCD_LCM_PARSER_BODY(int, ListEvalIntsubTop, EvalIntsubTop, ReadMatrixInt, op, 0)
}

complex CBC_lcm(char *SRC) {
    return parser_cplx(SRC, Cplx_fLCM);
}

double CBD_lcm(char *SRC) {
    return parser_dbl(SRC, fLCM);
}

int CBI_lcm(char *SRC) {
    return parser_int(SRC, fLCMint);
}

complex CBC_gcd(char *SRC) {
    return parser_cplx(SRC, Cplx_fGCD);
}

double CBD_gcd(char *SRC) {
    return parser_dbl(SRC, CB_gcd_dbl);
}

int CBI_gcd(char *SRC) {
    return parser_int(SRC, CB_gcd_int);
}
