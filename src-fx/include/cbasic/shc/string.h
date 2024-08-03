/*------------------------------------------------------*/
/* SH SERIES C Compiler Ver. 1.0                        */
/* Copyright (c) 1992 Hitachi,Ltd.                      */
/* Licensed material of Hitachi,Ltd.                    */
/*------------------------------------------------------*/
/*****************************************************************/
/* SPEC ;                                                        */
/*   NAME = string :                                             */
/*   FUNC =                                                      */
/*          ;                                                    */
/*                                                               */
/*                                                               */
/*                                                               */
/*   CLAS = UNIT ;                                               */
/*   LINK =                                                      */
/*   NOTE =                                                      */
/*          ;                                                    */
/*                                                               */
/*   END ;                                                       */
/*****************************************************************/
#ifndef _STRING
#define _STRING

#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#include <builtin.h>
#endif
__attribute__((renesas)) extern void *memcpy(void *, const void *, size_t);
__attribute__((renesas)) extern void *memmove(void *, const void *, size_t);
__attribute__((renesas)) extern char *strcpy(char *, const char *);
__attribute__((renesas)) extern char *strncpy(char *, const char *, size_t);
__attribute__((renesas)) extern char *strcat(char *, const char *);
__attribute__((renesas)) extern char *strncat(char *, const char *,size_t);
__attribute__((renesas)) extern int  memcmp(const void *, const void *,size_t);
__attribute__((renesas)) extern int  strcmp(const char *, const char *);
__attribute__((renesas)) extern int  strncmp(const char *, const char *, size_t);
__attribute__((renesas)) extern void *memchr(const void *, int, size_t);
__attribute__((renesas)) extern char *strchr(const char *, int);
__attribute__((renesas)) extern size_t strcspn(const char *, const char *);
__attribute__((renesas)) extern char *strpbrk(const char *, const char *);
__attribute__((renesas)) extern char *strrchr(const char *, int);
__attribute__((renesas)) extern size_t strspn(const char *, const char *);
__attribute__((renesas)) extern char *strstr(const char *, const char *);
__attribute__((renesas)) extern char *strtok(char *, const char *);
__attribute__((renesas)) extern void *memset(void *, int, size_t);
__attribute__((renesas)) extern char *strerror(int);
__attribute__((renesas)) extern size_t strlen(const char *);
#ifdef __cplusplus
}
#endif

// #define strcpy(s1,s2) _builtin_strcpy(s1,s2)
// #define strcmp(s1,s2) _builtin_strcmp(s1,s2)

#endif
