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
extern float _builtin_fabsf(float);
extern double _builtin_fabs(double);
extern float _builtin_sqrtf(float);
extern double _builtin_sqrt(double);

/* follow private.h */
extern void _builtin_fsca(long ,float *,float *);
extern float _builtin_fsrra(float);

/* follow smachine.h */
extern void _builtin_set_imask(int);
extern int _builtin_get_imask(void);
extern void _builtin_set_cr(int);
extern int _builtin_get_cr(void);
extern void _builtin_set_vbr(void *);
extern void *_builtin_get_vbr(void);
extern void _builtin_sleep(void);

/* follow string.h */
extern char *_builtin_strcpy(char *, const char *);
extern int _builtin_strcmp(const char *, const char *);

/* follow umachine.h */
extern void _builtin_set_gbr(void *);
extern void *_builtin_get_gbr(void);
extern unsigned char _builtin_gbr_read_byte(int);
extern unsigned short _builtin_gbr_read_word(int);
extern unsigned long _builtin_gbr_read_long(int);
extern void _builtin_gbr_write_byte(int, unsigned char);
extern void _builtin_gbr_write_word(int, unsigned short);
extern void _builtin_gbr_write_long(int, unsigned long);
extern void _builtin_gbr_and_byte(int, unsigned char);
extern void _builtin_gbr_or_byte(int, unsigned char);
extern void _builtin_gbr_xor_byte(int, unsigned char);
extern int _builtin_gbr_tst_byte(int, unsigned char);
extern int _builtin_tas(char *);
extern int _builtin_trapa(int);
extern int _builtin_macw(short *, short *, unsigned int);
extern int _builtin_macwl(short *, short *, unsigned int, unsigned int);
extern int _builtin_macl(int *, int *, unsigned int);
extern int _builtin_macll(int *, int *, unsigned int, unsigned int);
extern int _builtin_trapa_svc(...);
extern void _builtin_prefetch(void *);
extern void _builtin_set_fpscr(int);
extern int _builtin_get_fpscr(void);
extern float _builtin_fipr(float a1[], float b1[]);
extern void _builtin_ftrv(float a1[], float b1[]);
extern void _builtin_ftrvadd(float a1[], float b1[], float c1[]);
extern void _builtin_ftrvsub(float a1[], float b1[], float c1[]);
extern void _builtin_mtrx4mul(float [][4], float [][4]);
extern void _builtin_mtrx4muladd(float [][4], float [][4], float [][4]);
extern void _builtin_mtrx4mulsub(float [][4], float [][4], float [][4]);
extern void _builtin_ld_ext(float [][4]);
extern void _builtin_st_ext(float [][4]);
extern void _builtin_add4(float a1[], float b1[], float c1[]);
extern void _builtin_sub4(float a1[], float b1[], float c1[]);
extern void _builtin_trace(int);
}
#endif /* #ifdef __cplusplus */

#endif /* #ifndef _BUILTIN */
