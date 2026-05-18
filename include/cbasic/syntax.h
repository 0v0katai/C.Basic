#ifndef CBASIC_SYNTAX_H
#define CBASIC_SYNTAX_H

#define syntax_check_comma(ret) \
    if (SRC[ExecPtr] != ',') {  \
        CB_Error(SyntaxERR);    \
        return ret;             \
    }                           \
    ExecPtr++

#define syntax_skip(c) \
    if (SRC[ExecPtr] == c) ExecPtr++

#endif /* CBASIC_SYNTAX_H */
