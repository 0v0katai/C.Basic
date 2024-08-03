/*------------------------------------------------------*/
/* SH SERIES C Compiler Ver. 1.1                        */
/* Copyright (c) 1992 Hitachi,Ltd.                      */
/* Licensed material of Hitachi,Ltd.                    */
/*------------------------------------------------------*/
/*****************************************************************/
/* SPEC ;                                                        */
/*   NAME = mathf                                                */
/*   FUNC =                                                      */
/*          ;                                                    */
/*                                                               */
/*                                                               */
/*                                                               */
/*   CLAS = UNIT ;                                               */
/*   END ;                                                       */
/*****************************************************************/
#ifndef _MATHF
#define _MATHF

#ifdef __cplusplus
extern "C" {
#include <builtin.h>
#endif
__attribute__((renesas)) extern float frexpf(float, int *);
__attribute__((renesas)) extern float ldexpf(float, int);
__attribute__((renesas)) extern float modff(float, float *);
__attribute__((renesas)) extern float ceilf(float);
#if defined(_SH2E)|defined(_SH3E)|defined(_SH4)
#ifdef _FPD
#define  fabsf  _builtin_fabs
#else
#define  fabsf  _builtin_fabsf
#endif
#else
__attribute__((renesas)) extern float fabsf(float);
#endif
__attribute__((renesas)) extern float floorf(float);
__attribute__((renesas)) extern float fmodf(float, float);
__attribute__((renesas)) extern float acosf(float);
__attribute__((renesas)) extern float asinf(float);
__attribute__((renesas)) extern float atanf(float);
__attribute__((renesas)) extern float atan2f(float,float);
__attribute__((renesas)) extern float cosf(float);
__attribute__((renesas)) extern float sinf(float);
__attribute__((renesas)) extern float tanf(float);
__attribute__((renesas)) extern float coshf(float);
__attribute__((renesas)) extern float tanhf(float);
__attribute__((renesas)) extern float sinhf(float);
__attribute__((renesas)) extern float expf(float);
__attribute__((renesas)) extern float logf(float);
__attribute__((renesas)) extern float log10f(float);
__attribute__((renesas)) extern float powf(float,float);
#if defined(_SH3E)|defined(_SH4)
#ifdef _FPD
#define  sqrtf  _builtin_sqrt
#else
#define  sqrtf  _builtin_sqrtf
#endif
#else
__attribute__((renesas)) extern float sqrtf(float);
#endif
#ifdef __cplusplus
}
#endif

#endif
