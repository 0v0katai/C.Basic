/*------------------------------------------------------*/
/* SH SERIES C Compiler Ver. 6.0                        */
/* Copyright (c) 1992, 2000 Hitachi,Ltd.                */
/* Licensed material of Hitachi,Ltd.                    */
/*------------------------------------------------------*/
/*****************************************************************/
/* SPEC ;                                                        */
/*   NAME = builtin :                                            */
/*                                                               */
/*   FUNC = this header file do the following functions;         */
/*          (1)builtin function prototype define;                */
/*                                                               */
/*   CLAS = UNIT ;                                               */
/*                                                               */
/*   END ;                                                       */
/*****************************************************************/

#ifndef _BUILTIN
#define _BUILTIN

#ifdef __cplusplus
extern "C" {
/* follow math.h & mathf.h */
__attribute__((renesas)) extern float _builtin_fabsf(float);
__attribute__((renesas)) extern double _builtin_fabs(double);
__attribute__((renesas)) extern float _builtin_sqrtf(float);
__attribute__((renesas)) extern double _builtin_sqrt(double);

/* follow private.h */
__attribute__((renesas)) extern void _builtin_fsca(long ,float *,float *);
__attribute__((renesas)) extern float _builtin_fsrra(float);

/* follow smachine.h */
__attribute__((renesas)) extern void _builtin_set_imask(int);
__attribute__((renesas)) extern int _builtin_get_imask(void);
__attribute__((renesas)) extern void _builtin_set_cr(int);
__attribute__((renesas)) extern int _builtin_get_cr(void);
__attribute__((renesas)) extern void _builtin_set_vbr(void *);
__attribute__((renesas)) extern void *_builtin_get_vbr(void);
__attribute__((renesas)) extern void _builtin_sleep(void);

/* follow string.h */
__attribute__((renesas)) extern char *_builtin_strcpy(char *, const char *);
__attribute__((renesas)) extern int _builtin_strcmp(const char *, const char *);

/* follow umachine.h */
__attribute__((renesas)) extern void _builtin_set_gbr(void *);
__attribute__((renesas)) extern void *_builtin_get_gbr(void);
__attribute__((renesas)) extern unsigned char _builtin_gbr_read_byte(int);
__attribute__((renesas)) extern unsigned short _builtin_gbr_read_word(int);
__attribute__((renesas)) extern unsigned long _builtin_gbr_read_long(int);
__attribute__((renesas)) extern void _builtin_gbr_write_byte(int, unsigned char);
__attribute__((renesas)) extern void _builtin_gbr_write_word(int, unsigned short);
__attribute__((renesas)) extern void _builtin_gbr_write_long(int, unsigned long);
__attribute__((renesas)) extern void _builtin_gbr_and_byte(int, unsigned char);
__attribute__((renesas)) extern void _builtin_gbr_or_byte(int, unsigned char);
__attribute__((renesas)) extern void _builtin_gbr_xor_byte(int, unsigned char);
__attribute__((renesas)) extern int _builtin_gbr_tst_byte(int, unsigned char);
__attribute__((renesas)) extern int _builtin_tas(char *);
__attribute__((renesas)) extern int _builtin_trapa(int);
__attribute__((renesas)) extern int _builtin_macw(short *, short *, unsigned int);
__attribute__((renesas)) extern int _builtin_macwl(short *, short *, unsigned int, unsigned int);
__attribute__((renesas)) extern int _builtin_macl(int *, int *, unsigned int);
__attribute__((renesas)) extern int _builtin_macll(int *, int *, unsigned int, unsigned int);
__attribute__((renesas)) extern int _builtin_trapa_svc(...);
__attribute__((renesas)) extern void _builtin_prefetch(void *);
__attribute__((renesas)) extern void _builtin_set_fpscr(int);
__attribute__((renesas)) extern int _builtin_get_fpscr(void);
__attribute__((renesas)) extern float _builtin_fipr(float a1[], float b1[]);
__attribute__((renesas)) extern void _builtin_ftrv(float a1[], float b1[]);
__attribute__((renesas)) extern void _builtin_ftrvadd(float a1[], float b1[], float c1[]);
__attribute__((renesas)) extern void _builtin_ftrvsub(float a1[], float b1[], float c1[]);
__attribute__((renesas)) extern void _builtin_mtrx4mul(float [][4], float [][4]);
__attribute__((renesas)) extern void _builtin_mtrx4muladd(float [][4], float [][4], float [][4]);
__attribute__((renesas)) extern void _builtin_mtrx4mulsub(float [][4], float [][4], float [][4]);
__attribute__((renesas)) extern void _builtin_ld_ext(float [][4]);
__attribute__((renesas)) extern void _builtin_st_ext(float [][4]);
__attribute__((renesas)) extern void _builtin_add4(float a1[], float b1[], float c1[]);
__attribute__((renesas)) extern void _builtin_sub4(float a1[], float b1[], float c1[]);
__attribute__((renesas)) extern void _builtin_trace(int);
}
#endif /* #ifdef __cplusplus */

#endif /* #ifndef _BUILTIN */
