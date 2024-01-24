#include <ctype.h>
#include <fxlib.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <timer.h>
#include "KeyScan.h"
#include "CB_io.h"
#include "CB_inp.h"
#include "CB_glib.h"
#include "CB_glib2.h"
#include "CB_Eval.h"
#include "CB_file.h"
#include "CB_edit.h"
#include "CB_setup.h"
#include "CB_Time.h"

#include "CB_interpreter.h"
#include "CBI_interpreter.h"
#include "CB_error.h"
#include "fx_syscall.h"

//-----------------------------------------------------------------------------
// Casio Basic Gloval variable
//-----------------------------------------------------------------------------

String Str*;


char Str[STRLENMAX * 20];
char StrPtr[20];

//-----------------------------------------------------------------------------

int StrCmp( char *str1, char *str2 ) {
	return strcmp(str1,str2);
}
int StrSrc( char *str, char *src ) {
	char *p;
	p=strchr( str, src );
	if (  p==NULL ) return 0;
	return p-str;
}
int StrJoin( char *str1, char *str2 ) {
	strcat( str1, str2);
	return 1;
}
int StrLen( char *str ) {
	return strlenOp( str ) ;
}
int StrLeft( char *str, int n ) {
}
int StrMid( char *str, int n, int m ) {
}
int StrRight( char *str, int n ) {
}
int StrInv( char *str ) {
}
int StrRotate( char *str, int n ) {
}
int StrShift( char *str, int n ) {
}
int StrLwr( char *str ) {
}
int StrUpr( char *str ) {
}


//-----------------------------------------------------------------------------
// Casio Basic
//-----------------------------------------------------------------------------
void CB_Exp( char *SRC ) {
	double value=CB_EvalDbl( SRC );
	Graph_Eval(str);
}
void CB_ExpToStr( char *SRC ) {
}

void CB_StrCmp( char *SRC ) {
}
void CB_StrSrc( char *SRC ) {
}

void CB_StrJoin( char *SRC ) {
}
void CB_StrLen( char *SRC ) {
}
void CB_StrLeft( char *SRC ) {
}
void CB_StrMid( char *SRC ) {
}
void CB_StrRight( char *SRC ) {
}

void CB_StrInv( char *SRC ) {
}
void CB_StrRotate( char *SRC ) {
}
void CB_StrShift( char *SRC ) {
}
void CB_StrLwr( char *SRC ) {
}
void CB_StrUpr( char *SRC ) {
}



