#include "CB.h"
#include "syntax.h"

#define GCD_LCM_PARSER_BODY(TYPE, IDENTITY, LIST_EVAL, SCALAR_EVAL, MATRIX_READ, OP, ERROR_RET) \
    TYPE result = (IDENTITY); \
    dspflag = 0; \
    if (CB_MatListAnsreg >= 28) CB_MatListAnsreg = 28; \
    LIST_EVAL(SRC); \
    if (dspflag >= 3) { \
        if (dspflag != 4) { \
            CB_Error(ArgumentERR); \
            return (ERROR_RET); \
        } \
        int list_idx = CB_MatListAnsreg; \
        int size = MatAry[list_idx].SizeA; \
        int base = MatAry[list_idx].Base; \
        for (int ptr = base; ptr < size + base; ptr++) \
            result = OP(result, MATRIX_READ(list_idx, ptr, base)); \
        dspflag = 2; \
    } else { \
        do { \
            syntax_skip(','); \
            result = OP(result, SCALAR_EVAL(SRC)); \
        } while (SRC[ExecPtr] == ','); \
    } \
    syntax_skip(')'); \
    return result;

static complex parser_cplx(char *SRC, complex identity, complex (*op)(complex, complex)) {
    GCD_LCM_PARSER_BODY(complex, identity, Cplx_ListEvalsubTop, Cplx_EvalsubTop, Cplx_ReadMatrix, op, Int2Cplx(0))
}

static double parser_dbl(char *SRC, double identity, double (*op)(double, double)) {
    GCD_LCM_PARSER_BODY(double, identity, ListEvalsubTop, EvalsubTop, ReadMatrix, op, 0.0)
}

static int parser_int(char *SRC, int identity, int (*op)(int, int)) {
    GCD_LCM_PARSER_BODY(int, identity, ListEvalIntsubTop, EvalIntsubTop, ReadMatrixInt, op, 0)
}

complex CBC_lcm(char *SRC) {
    return parser_cplx(SRC, Int2Cplx(1), Cplx_fLCM);
}

double CBD_lcm(char *SRC) {
    return parser_dbl(SRC, 1.0, fLCM);
}

int CBI_lcm(char *SRC) {
    return parser_int(SRC, 1, fLCMint);
}

complex CBC_gcd(char *SRC) {
    return parser_cplx(SRC, Int2Cplx(0), Cplx_fGCD);
}

double CBD_gcd(char *SRC) {
    return parser_dbl(SRC, 0.0, CB_gcd_dbl);
}

int CBI_gcd(char *SRC) {
    return parser_int(SRC, 0, CB_gcd_int);
}
