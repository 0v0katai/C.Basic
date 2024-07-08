/*------------------------------------------------------*/
/* SH SERIES C Compiler Ver. 1.0                        */
/* Copyright (c) 1992 Hitachi,Ltd.                      */
/* Licensed material of Hitachi,Ltd.                    */
/*------------------------------------------------------*/
/*****************************************************************/
/* SPEC ;                                                        */
/*   NAME = ctype :                                              */
/*   FUNC =                                                      */
/*          ;                                                    */
/*                                                               */
/*                                                               */
/*                                                               */
/*   CLAS = UNIT ;                                               */
/*   END ;                                                       */
/*****************************************************************/
#ifndef _CTYPE
#define _CTYPE

#ifdef __cplusplus
extern "C" {
#endif

__attribute__((renesas)) extern unsigned char _ctype[];

__attribute__((renesas)) extern int isalnum(int);
__attribute__((renesas)) extern int isalpha(int);
__attribute__((renesas)) extern int iscntrl(int);
__attribute__((renesas)) extern int isdigit(int);
__attribute__((renesas)) extern int isgraph(int);
__attribute__((renesas)) extern int islower(int);
__attribute__((renesas)) extern int isprint(int);
__attribute__((renesas)) extern int ispunct(int);
__attribute__((renesas)) extern int isspace(int);
__attribute__((renesas)) extern int isupper(int);
__attribute__((renesas)) extern int isxdigit(int);
__attribute__((renesas)) extern int tolower(int);
__attribute__((renesas)) extern int toupper(int);
#ifdef __cplusplus
}
#endif

#define _UPPER   0x01
#define _LOWER   0x02
#define _DIGIT   0x04
#define _SPACE   0x08
#define _PUNCT   0x10
#define _CNTRL   0x20
#define _WHITE   0x40
#define _HEX     0x80

#define isalnum(c)  (_ctype[(c) & 0xff] & (_UPPER | _LOWER | _DIGIT))
#define isalpha(c)  (_ctype[(c) & 0xff] & (_UPPER | _LOWER))
#define iscntrl(c)  (_ctype[(c) & 0xff] & _CNTRL)
#define isdigit(c)  (_ctype[(c) & 0xff] & _DIGIT)
#define isgraph(c)  (_ctype[(c) & 0xff] & (_UPPER | _LOWER | _DIGIT | _PUNCT))
#define islower(c)  (_ctype[(c) & 0xff] & _LOWER)
#define isprint(c)  (_ctype[(c) & 0xff] & (_UPPER|_LOWER|_DIGIT|_PUNCT|_SPACE))
#define ispunct(c)  (_ctype[(c) & 0xff] & _PUNCT)
#define isspace(c)  (_ctype[(c) & 0xff] & _WHITE)
#define isupper(c)  (_ctype[(c) & 0xff] & _UPPER)
#define isxdigit(c) (_ctype[(c) & 0xff] & _HEX)
#define tolower(c)  (isupper(c) ? ((c)-'A'+'a') : (c))
#define toupper(c)  (islower(c) ? ((c)-'a'+'A') : (c))

#endif
