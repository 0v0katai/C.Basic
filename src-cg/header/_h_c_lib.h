/*------------------------------------------------------*/
/* SH SERIES C/C++ Compiler Ver. 6.0                    */
/* Copyright (c) 1992,2000 Hitachi,Ltd.                 */
/* Licensed material of Hitachi,Ltd.                    */
/*------------------------------------------------------*/
/***********************************************************************/
/* SPEC;                                                               */
/*  NAME = _h_c_lib.h :                                                */
/*                                                                     */
/*  FUNC = Include file for SH SERIES C/C++ Compiler environment       */
/*                                                                     */
/*  CLAS = UNIT;                                                       */
/*                                                                     */
/* END;                                                                */
/***********************************************************************/

#ifndef _H_C_LIB
#define _H_C_LIB

#ifdef __cplusplus
extern "C"{
#endif
extern void _CALL_INIT(void); /* for global class object initial processing */
extern void _CALL_END(void);  /* for global class object post-processing */
#ifdef __cplusplus
}
#endif /* #ifdef __cplusplus */

#endif /* #ifndef _H_C_LIB */
